#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../constants.h"

using namespace components;

enum HeaterMode{
	TUNING,
	NORMALOP
};

class HeaterController:public Component{
public:
    HeaterController(const HeaterControllerConfig& config);
    void Initialize();
    void TransitionMode();
    void StartTuning();
    void StopTuning();
    void TunePid();
    void ToggleHeaters();
    void TurnOn();
    void TurnOff();
    void ReadTemperatures();
    Array<HeaterResult,HEATER_COUNT> GetResults();
    void Print();

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