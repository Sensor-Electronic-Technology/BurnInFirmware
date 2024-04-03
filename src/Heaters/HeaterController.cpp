#include "HeaterController.hpp"

HeaterController::HeaterController(const HeaterControllerConfig& config)
        :Component(),readInterval(config.readInterval),configuration(config){

    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->TuningComplete(result);
    };

    ComHandler::MapHeaterResponseCallback([&](Response response){
        this->HandleResponse(response);
    });

    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        //Heater heater(config.heaterConfigs[i]);
        this->heaters[i]=Heater(config.heaterConfigs[i]);
        // HeaterResult result;
        this->results[i]=HeaterResult();
        //this->heaters[i].SetConfiguration(config.heaterConfigs[i]);
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].MapTurningComplete(this->tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
    }
}

HeaterController::HeaterController():Component(){
    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->TuningComplete(result);
    };

    ComHandler::MapHeaterResponseCallback([&](Response response){
        this->HandleResponse(response);
    });

}

void HeaterController::Setup(const HeaterControllerConfig& config){
    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i]=Heater(config.heaterConfigs[i]);
        this->results[i]=HeaterResult();
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].MapTurningComplete(this->tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
    }
}

void HeaterController::Initialize(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].Initialize();
        
    }
    for(uint8_t i=0;i<100;i++){
        this->ReadTemperatures();
    }

    // this->printTimer.onInterval([&]{
    //     this->Print();
    // },2500);
    Serial.println(F("Heater Controller Initialize Timer Setup"));
    this->readTimer.onInterval([&]{
        this->ReadTemperatures();
    },this->readInterval);
    RegisterChild(this->readTimer);
}

void HeaterController::HandleResponse(Response response){
    if(response==Response::HEATER_SAVE){
        //update and save configuration
        //then request restart
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            auto newPid=this->tuningResults.results[i];
            this->configuration.UpdateHeaterPid(newPid);
            this->heaters[newPid.heaterNumber-1].UpdatePid(newPid);
            FileManager::Save(&this->configuration,PacketType::HEATER_CONFIG);
            //Send new config to PC!  ComHandler::SendConfig(&config);  //I thought i had this already
        }
    }else if(response==Response::HEATER_CANCEL){
        //clear tuning results
        //wait for mode change
        this->tuningResults.Clear();
    }
}

void HeaterController::TuningComplete(HeaterTuneResult result){
    if(result.heaterNumber<HEATER_COUNT && result.heaterNumber>-1){
        HeaterTunePacket tuning;
        //TODO: replace this->tuningResults with HeaterTunePacket
        this->tuningResults.results[result.heaterNumber-1]=result;
        tuning.set(result.heaterNumber-1,result);
        bool tuningComplete=true;
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            tuningComplete&=this->tuningResults.results[i].complete;
        }
        if(tuningComplete){
            //Send values
            this->tuningCompleted=true;
            this->tuningStateMachine.triggerEvent(TUNE_COMPLETE);
            ComHandler::MsgPacketSerializer(tuning,PacketType::HEATER_TUNE_COMPLETE);
            //ComHandler::SendRequest(PacketType::HEATER_REQUEST,F("Tuning Completed. Save new PID parameters?",this->tuningResults);
        }else{
            if(result.complete){
                StationLogger::Log(LogLevel::INFO,true,false,
                        "Heater %d tuning complete. Please wait for other heaters to finish tuning \n New Values: kp: %f ki: %f kd: %f",
                        result.heaterNumber,result.kp,result.ki,result.kd);
            }
        }
    }
}

void HeaterController::ChangeSetPoint(uint8_t setPoint){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].ChangeSetpoint(setPoint);
    }
}

void HeaterController::StartTuning(){
    this->isTuning=true;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StartTuning();
    }
}

void HeaterController::StopTuning(){
    this->isTuning=false;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StopTuning();
    }
}

void HeaterController::TuningRun(){
    this->isTuning=true;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunAutoTune();
        this->isTuning&=this->heaters[i].IsTuning();
    }
}

void HeaterController::Run(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunPid();
    }
}

void HeaterController::ToggleHeaters(){
    if(this->heaterState==HeatState::On){
        this->TurnOff();
    }else{
        this->TurnOn();
    }
}

void HeaterController::TurnOn(){
    this->heaterState=HeatState::On;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOn();
    }
}

void HeaterController::TurnOff(){
    this->heaterState=HeatState::Off;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOff();
    }
}

void HeaterController::ReadTemperatures(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        results[i]=heaters[i].Read();
        //results[i]=heaters[i]->GetHeaterResult();
    }
}

bool HeaterController::TempOkay(){
    bool okay=true;
    for(auto result:this->results){
        okay&=result.tempOkay;
    }
    return okay;
}

void HeaterController::GetResults(HeaterResult* fill){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
       fill[i]=this->heaters[i].GetHeaterResult();
    }
}

void HeaterController::Print(){
    Serial.print("H[1]=");Serial.print(results[0].temperature);
    Serial.print(" , H[2]=");Serial.print(results[0].temperature);
    Serial.print(" , H[3]=");Serial.println(results[0].temperature);
}

void HeaterController::RunHeating(){
    this->heatersStateMachine.Run();
}

void HeaterController::RunAutoTune(){
    this->tuningStateMachine.Run();
}

void HeaterController::privateLoop(){
    this->modeStateMachine.Run();
}