#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "../Configuration/HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../constants.h"

using namespace components;

enum HeaterMode{
	TUNING,
	NORMALOP
};

class HeaterController:public Component{
public:
    HeaterController(const HeaterControllerConfig& config)
    :Component(),readInterval(config.readInterval){
        for(int i=0;i<HEATER_COUNT;i++){
            Heater* heater=new Heater(config.heaterConfigs[i]);
            this->heaters.push_back(heater);
            RegisterChild(heater);
            HeaterResult result;
            this->results.push_back(result);
        }
    }

    void Initialize(){
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

    void TransitionMode(){
        switch(this->nextMode){
            case HeaterMode::TUNING:{
                this->TurnOff();
                break;
            }
            case HeaterMode::NORMALOP:{
                this->StopTuning();
                break;
            };
        }
    }


    void StartTuning(){
        this->isTuning=true;
        for(auto heater:this->heaters){
            heater->StartTuning();
        }
    }

    void StopTuning(){
        this->isTuning=false;
        for(auto heater:this->heaters){
            heater->StopTuning();
        }
    }

    void TunePid(){
        if(this->isTuning){
            for(auto heater:this->heaters){
                heater->TunePidV2();
                this->isTuning&=(bool)(heater->IsTuning());
            }
            if(!this->isTuning){
                Serial.println("Completed");
            }
        }
    }

    void ToggleHeaters(){
        if(this->heaterState==HeaterState::On){
            this->TurnOff();
        }else{
            this->TurnOn();
        }
    }

    void TurnOn(){
        this->heaterState=HeaterState::On;
        for(auto heater:this->heaters){
            heater->TurnOn();
        }
    }

    void TurnOff(){
        this->heaterState=HeaterState::Off;
        for(auto heater:this->heaters){
            heater->TurnOff();
        }
    }

    void ReadTemperatures(){
        for(int i=0;i<HEATER_COUNT;i++){
            results[i]=heaters[i]->Read();
            //results[i]=heaters[i]->GetHeaterResult();
        }
    }

    Array<HeaterResult,HEATER_COUNT> GetResults(){
        for(int i=0;i<HEATER_COUNT;i++){
            this->results[i]=this->heaters[i]->GetHeaterResult();
        }
        return this->results;
    }

    void Print(){
        Serial.print("H[1]=");Serial.print(results[0].temperature);
        Serial.print(" , H[2]=");Serial.print(results[0].temperature);
        Serial.print(" , H[3]=");Serial.println(results[0].temperature);
    }

private:
    Array<Heater*,HEATER_COUNT> heaters;
    Array<HeaterResult,HEATER_COUNT> results;
    Timer  readTimer,printTimer;
    HeaterState heaterState=HeaterState::Off;
    HeaterMode mode,nextMode;
    unsigned long readInterval;
    bool isTuning;

    void privateLoop(){ 
        if(this->nextMode!=this->mode){

        }
    }
};