#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../Configuration/ConfigurationManager.hpp"
#include "../constants.h"

using namespace components;

class HeaterController:public Component{
public:
    HeaterController(const HeaterControllerConfig& config);
    void Initialize();
    void StartTuning();
    void StopTuning();
    void TunePid();
    void ToggleHeaters();
    void TurnOn();
    void TurnOff();
    void ReadTemperatures();
    Array<HeaterResult,HEATER_COUNT> GetResults();
    void Print();
    void Printv2(){
        Serial.println(filenames[0]);
        Serial.println(filenames[1]);
        Serial.println(filenames[2]);
    }

    void TransitionMode();
    void TransitionNormToTune();
    void TransitionTuneToNorm();

private:
    Array<Heater*,HEATER_COUNT> heaters;
    Array<HeaterResult,HEATER_COUNT> results;
    Timer  readTimer,printTimer;
    HeaterState heaterState=HeaterState::Off;
    HeaterMode mode,nextMode;
    HeaterControllerConfig configuration;
    unsigned long readInterval;
    bool isTuning;

    virtual void privateLoop() override;
};