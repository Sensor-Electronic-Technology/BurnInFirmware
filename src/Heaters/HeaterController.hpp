#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../StateMachine/StateMachine.hpp"
#include "../Files/FileManager.hpp"
#include "heater_constants.h"
#include "../Task.hpp"
#include "../Communication/TuningSerialData.hpp"



using namespace components;

class HeaterController:public Component{
    typedef void(HeaterController::*ModeRun)(void);
    typedef void(HeaterController::*TuneStateRun)(void);
    typedef void(HeaterController::*HeaterStateRun)(void);
public:
    HeaterController(const HeaterControllerConfig& config);
    HeaterController();
    void Setup(const HeaterControllerConfig& config);
    void Initialize();
    void ReadTemperatures();
    bool TempOkay();
    bool ChangeSetPoint(uint8_t setPoint);
    int GetSetPoint();
    void GetResults(HeaterResult* fill);
    void Run();
    void HeatingRun();
    void TuneRun();

    inline bool IsTuning(){
        return this->isTuning;
    }

#pragma region Mode

    //Modes
    void RunAutoTune();
    void RunHeating();

    //Mode Transitions
    bool SwitchToAutoTune();
    bool SwitchToHeating();

#pragma region Heating
    //Heating States
    void RunOn();
    void RunWarmup();
    void RunIdle();
    void TurnOn();
    void TurnOff();

    void OnTurnOn(); 
    void OnTurnOff();

    void ToggleHeaters();
#pragma endregion


#pragma region Tuning
    //Tuning States
    void TuningRun();
    //Tuning Commands
    bool StartTuning();
    bool StopTuning();
    bool SaveTuning();
    bool DiscardTuning();

    //Tuning Callbacks
    void HeaterRunCompleteHandler(HeaterTuneResult result);
    //Tune Transitions
    void OnStartTuning();
    void OnStopTuning();

    void OnSaveTuning();
    void OnDiscardTuning();
#pragma endregion


private:
    Heater* heaters[HEATER_COUNT];
    HeaterResult results[HEATER_COUNT];
    AutoTuneResults tuningResults;
    StationTimer  readTimer,tuningComTimer;
    Task<HeaterMode>  mode;
    Task<HeaterState> hState;
    Task<TuneState>   tState;
    HeatState heaterState=HeatState::Off;
    TuningSerialData tuningSerialData;
    HeaterControllerConfig configuration;
    TuningCompleteCallback tuningCompleteCbk=[](HeaterTuneResult){};
    unsigned long readInterval;
    bool isTuning=false;
    bool tuningCompleted=false;
    unsigned long tuningElapsed=0;
    int tempSp=DEFAULT_TEMPSP;
    virtual void privateLoop() override;

    ModeRun modeRun[MODE_COUNT];
    HeaterStateRun heaterStateRun[HEATER_STATE_COUNT];
    TuneStateRun tuneStateRun[TUNE_STATE_COUNT];

};