#include "HeaterController.hpp"

HeaterController::HeaterController(const HeaterControllerConfig& config)
:Component(),readInterval(config.readInterval),configuration(config){
    for(int i=0;i<HEATER_COUNT;i++){
        Heater* heater=new Heater(config.heaterConfigs[i]);
        this->heaters.push_back(heater);
        RegisterChild(heater);
        HeaterResult result;
        this->results.push_back(result);
    }
}

void HeaterController::Initialize(){
    for(auto heater:this->heaters){
        heater->Initialize();
    }
    for(int i=0;i<100;i++){
        this->ReadTemperatures();
    }

    this->printTimer.onInterval([&]{
        this->Print();
    },2500);

    this->readTimer.onInterval([&]{
        this->ReadTemperatures();
    },1000);
    RegisterChild(this->readTimer);
    RegisterChild(this->printTimer);
}

void HeaterController::TransitionMode(){

}


void HeaterController::StartTuning(){
    this->isTuning=true;
    for(auto heater:this->heaters){
        heater->StartTuning();
    }
}

void HeaterController::StopTuning(){
    this->isTuning=false;
    for(auto heater:this->heaters){
        heater->StopTuning();
    }
}

void HeaterController::TunePid(){
    if(this->isTuning){
        for(auto heater:this->heaters){
            heater->RunAutoTune();
            this->isTuning&=(bool)(heater->IsTuning());
        }
        if(!this->isTuning){
            Serial.println("Completed");
        }
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
    for(auto heater:heaters){
        //heater->
    }
}