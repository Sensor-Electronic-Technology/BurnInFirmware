#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../Files/FileManager.hpp"
#include "../Logging/StationLogger.hpp"
#include "heater_constants.h"

using namespace components;

class HeaterController:public Component{
    typedef void(HeaterController::*RunMode)();
public:
    HeaterController(const HeaterControllerConfig& config);
    HeaterController();
    void Setup(const HeaterControllerConfig& config);
    void Initialize();
    void StartTuning();
    void StopTuning();
    void RunAutoTune();
    void Run();
    void ToggleHeaters();
    void TurnOn();
    void TurnOff();
    void ReadTemperatures();
    bool TempOkay();
    void ChangeSetPoint(int setPoint);
    void HandleResponse(Response response);
    void TuningComplete(HeaterTuneResult result);
    void ChangeMode(HeaterMode nextMode);
    void GetResults(HeaterResult* fill);
    void Print();
    void Printv2(){
        Serial.println(json_filenames[0]);
        Serial.println(json_filenames[1]);
        Serial.println(json_filenames[2]);
    }

    inline HeaterMode GetMode(){
        return this->heaterMode;
    }

    inline bool IsTuning(){
        return this->isTuning;
    }

    

private:
    Heater heaters[HEATER_COUNT];
    HeaterResult results[HEATER_COUNT];
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