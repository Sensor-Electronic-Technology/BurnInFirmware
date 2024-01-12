#include "HeaterController.hpp"

HeaterController::HeaterController(const HeaterControllerConfig& config)
        :Component(),readInterval(config.readInterval),configuration(config){

    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->TuningComplete(result);
    };

    for(int i=0;i<HEATER_COUNT;i++){
        Heater* heater=new Heater(config.heaterConfigs[i]);
        heater->MapTurningComplete(this->tuningCompleteCbk);
        this->heaters.push_back(heater);
        RegisterChild(heater);
        HeaterResult result;
        this->results.push_back(result);
    }

    this->run[HeaterMode::PID_RUN]=&HeaterController::Run;
    this->run[HeaterMode::ATUNE_RUN]=&HeaterController::RunAutoTune;

    this->responseCbk=[&](Response response){
        this->HandleResponse(response);
    };
    ComHandler::MapHeaterResponseCallback(this->responseCbk);
}

void HeaterController::Initialize(){
    
    for(auto heater:this->heaters){
        heater->Initialize();
        
    }
    for(int i=0;i<100;i++){
        this->ReadTemperatures();
    }

    // this->printTimer.onInterval([&]{
    //     this->Print();
    // },2500);

    this->readTimer.onInterval([&]{
        this->ReadTemperatures();
    },this->readInterval);
    RegisterChild(this->readTimer);
    //RegisterChild(this->printTimer);
}

void HeaterController::HandleResponse(Response response){
    if(response==Response::HEATER_SAVE){
        //update and save configuration
        //then request restart
        for(int i=0;i<HEATER_COUNT;i++){
            auto newPid=this->tuningResults.results[i];
            this->configuration.UpdateHeaterPid(newPid);
            this->heaters[newPid.heaterNumber-1]->UpdatePid(newPid);
            ConfigurationManager::SaveConfig(&this->configuration,PacketType::HEATER_CONFIG);
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
            ComHandler::SendRequest(PacketType::HEATER_REQUEST,"Tuning Completed. Save new PID parameters?",this->tuningResults);
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
        for(auto heater:this->heaters){
            heater->SwitchMode(nextMode);
        }
        this->isTuning=false;
    }
}

void HeaterController::StartTuning(){
   if(this->heaterMode==HeaterMode::ATUNE_RUN){
        this->isTuning=true;
        for(auto heater:this->heaters){
            heater->StartTuning();
        }
   }
}

void HeaterController::StopTuning(){
    if(this->heaterMode==HeaterMode::ATUNE_RUN){
        this->isTuning=false;
        for(auto heater:this->heaters){
            heater->StopTuning();
        }
   }
}

void HeaterController::RunAutoTune(){
    for(auto heater:this->heaters){
        heater->RunAutoTune();
        this->isTuning&=(bool)(heater->IsTuning());
    }
}

void HeaterController::Run(){
    for(auto heater:this->heaters){
        heater->Run();
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
    for(auto heater:this->heaters){
        heater->TurnOn();
    }
}

void HeaterController::TurnOff(){
    this->heaterState=HeaterState::Off;
    for(auto heater:this->heaters){
        heater->TurnOff();
    }
}

void HeaterController::ReadTemperatures(){
    for(int i=0;i<HEATER_COUNT;i++){
        results[i]=heaters[i]->Read();
        //results[i]=heaters[i]->GetHeaterResult();
    }
}

Array<HeaterResult,HEATER_COUNT> HeaterController::GetResults(){
    for(int i=0;i<HEATER_COUNT;i++){
        this->results[i]=this->heaters[i]->GetHeaterResult();
    }
    return this->results;
}

void HeaterController::Print(){
    Serial.print("H[1]=");Serial.print(results[0].temperature);
    Serial.print(" , H[2]=");Serial.print(results[0].temperature);
    Serial.print(" , H[3]=");Serial.println(results[0].temperature);
}

void HeaterController::privateLoop(){
    
}