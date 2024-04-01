#include "HeaterController.hpp"

HeaterController::HeaterController(const HeaterControllerConfig& config)
        :Component(),readInterval(config.readInterval),configuration(config){

    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->TuningComplete(result);
    };
    this->responseCbk=[&](Response response){
        this->HandleResponse(response);
    };
    ComHandler::MapHeaterResponseCallback(this->responseCbk);

    // for(int i=0;i<HEATER_COUNT;i++){
    //     Heater* heater=new Heater(config.heaterConfigs[i]);
    //     heater->MapTurningComplete(this->tuningCompleteCbk);
    //     this->heaters.push_back(heater);
    //     RegisterChild(heater);
    //     HeaterResult result;
    //     this->results.push_back(result);
    // }
    for(int i=0;i<HEATER_COUNT;i++){ 
        //Heater heater(config.heaterConfigs[i]);
        this->heaters[i]=Heater(config.heaterConfigs[i]);
        // HeaterResult result;
        this->results[i]=HeaterResult();
        //this->heaters[i].SetConfiguration(config.heaterConfigs[i]);
    }

    for(int i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].MapTurningComplete(this->tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
    }
    this->run[HeaterMode::PID_RUN]=&HeaterController::Run;
    this->run[HeaterMode::ATUNE_RUN]=&HeaterController::RunAutoTune;
}

HeaterController::HeaterController():Component(){
    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->TuningComplete(result);
    };
    this->responseCbk=[&](Response response){
        this->HandleResponse(response);
    };
    ComHandler::MapHeaterResponseCallback(this->responseCbk);   
}

void HeaterController::Setup(const HeaterControllerConfig& config){
    //Serial.println(F("HeaterController Setup"));
    for(int i=0;i<HEATER_COUNT;i++){ 
        //Heater heater(config.heaterConfigs[i]);
        this->heaters[i]=Heater(config.heaterConfigs[i]);
        // HeaterResult result;
        this->results[i]=HeaterResult();
        //this->heaters[i].SetConfiguration(config.heaterConfigs[i]);
    }

    for(int i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].MapTurningComplete(this->tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
    }
    this->run[HeaterMode::PID_RUN]=&HeaterController::Run;
    this->run[HeaterMode::ATUNE_RUN]=&HeaterController::RunAutoTune;
    //Serial.println(F("End HeaterController Setup"));
}

void HeaterController::Initialize(){
    //Serial.println(F("Heater Controller Initialize"));
    for(int i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].Initialize();
        
    }
    //Serial.println(F("Heater Controller Initialize ReadTemperatures"));
    for(int i=0;i<100;i++){
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
    //Serial.println(F("Heater Controller Initialize Completed"));
    //RegisterChild(this->printTimer);
}

void HeaterController::HandleResponse(Response response){
    if(response==Response::HEATER_SAVE){
        //update and save configuration
        //then request restart
        for(int i=0;i<HEATER_COUNT;i++){
            auto newPid=this->tuningResults.results[i];
            this->configuration.UpdateHeaterPid(newPid);
            this->heaters[newPid.heaterNumber-1].UpdatePid(newPid);
            FileManager::Save(&this->configuration,PacketType::HEATER_CONFIG);
        }
    }else if(response==Response::HEATER_CANCEL){
        //clear tuning results
        //wait for mode change
        this->tuningResults.Clear();
    }
}

void HeaterController::TuningComplete(HeaterTuneResult result){
    if(result.heaterNumber<HEATER_COUNT && result.heaterNumber>-1){
        this->tuningResults.results[result.heaterNumber-1]=result;
        bool tuningComplete=true;
        for(int i=0;i<HEATER_COUNT;i++){
            tuningComplete&=this->tuningResults.results[i].complete;
        }
        if(tuningComplete){
            //Send values
            this->tuningCompleted=true;
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

void HeaterController::ChangeMode(HeaterMode nextMode){
    if(this->heaterMode!=nextMode){
        for(int i=0;i<HEATER_COUNT;i++){
            this->heaters[i].SwitchMode(nextMode);
        }
        this->isTuning=false;
    }
}

void HeaterController::ChangeSetPoint(int setPoint){
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].ChangeSetpoint(setPoint);
    }
}

void HeaterController::StartTuning(){
    this->isTuning=true;
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StartTuning();
    }
}

void HeaterController::StopTuning(){
    this->isTuning=false;
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StopTuning();
    }
}

void HeaterController::RunAutoTune(){
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunAutoTune();
        this->isTuning&=(bool)(this->heaters[i].IsTuning());
    }
}

void HeaterController::Run(){
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunPid();
    }
}

void HeaterController::ToggleHeaters(){
    if(this->heaterState==HeaterState::On){
        this->TurnOff();
    }else{
        this->TurnOn();
    }
}

void HeaterController::TurnOn(){
    this->heaterState=HeaterState::On;
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOn();
    }
}

void HeaterController::TurnOff(){
    this->heaterState=HeaterState::Off;
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOff();
    }
}

void HeaterController::ReadTemperatures(){
    for(int i=0;i<HEATER_COUNT;i++){
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
    for(int i=0;i<HEATER_COUNT;i++){
       fill[i]=this->heaters[i].GetHeaterResult();
    }
}

void HeaterController::Print(){
    Serial.print("H[1]=");Serial.print(results[0].temperature);
    Serial.print(" , H[2]=");Serial.print(results[0].temperature);
    Serial.print(" , H[3]=");Serial.println(results[0].temperature);
}

void HeaterController::privateLoop(){
    
}