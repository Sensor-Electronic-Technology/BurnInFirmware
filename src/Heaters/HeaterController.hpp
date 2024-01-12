#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../Configuration/ConfigurationManager.hpp"
#include "../Logging/StationLogger.hpp"
#include "../constants.h"

using namespace components;

// struct TuningStatus{
//     bool h1Complete;
//     bool h2Complete;
//     bool h3Complete;
// };

class HeaterController:public Component{
    typedef void(HeaterController::*RunMode)();
public:
    HeaterController(const HeaterControllerConfig& config);
    void Initialize();
    void StartTuning();
    void StopTuning();
    void RunAutoTune();
    void Run();
    void ToggleHeaters();
    void TurnOn();
    void TurnOff();
    void ReadTemperatures();
    void HandleResponse(Response response);
    void TuningComplete(HeaterTuneResult result);
    Array<HeaterResult,HEATER_COUNT> GetResults();
    void Print();
    void Printv2(){
        Serial.println(filenames[0]);
        Serial.println(filenames[1]);
        Serial.println(filenames[2]);
    }

    void ChangeMode(HeaterMode nextMode);

private:
    Array<Heater*,HEATER_COUNT> heaters;
    Array<HeaterResult,HEATER_COUNT> results;
    AutoTuneResults tuningResults;
    Timer  readTimer,printTimer;
    HeaterState heaterState=HeaterState::Off;
    HeaterMode heaterMode=HeaterMode::PID_RUN;
    HeaterControllerConfig configuration;
    TuningCompleteCallback tuningCompleteCbk=[](HeaterTuneResult){};
    ResponseCallback responseCbk=[](Response){};
    unsigned long readInterval;
    bool isTuning=false;
    bool tuningCompleted=false;
    RunMode run[2];
    virtual void privateLoop() override;
};