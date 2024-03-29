#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include <avr/wdt.h>
#include "../Serializable.hpp"
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "../Communication/ComHandler.hpp"
#include "../Communication/SerialData.hpp"
#include "../Logging/StationLogger.hpp"
#include "../Files/FileManager.hpp"
#include "SaveState.hpp"
#include "StationState.hpp"
#include "../TestTimer/burn_timer_includes.h"
#include "../constants.h"

using namespace components;

enum StateTriggers{
    START_TEST,
    PAUSE_TEST,
    CONTINUE_TEST,
    SW_TUNE,
    SW_NORM,
    ACCECPT_TUNE,
    REJECT_TUNE    
};


class Controller:public Component{
public:
    Controller();

    //Init
    void LoadConfigurations();
    void SetupComponents();
    void CheckSavedState();

    //Transition Checks 
    bool CanTransitionTo(StationMode mode);

    //other
    void HandleCommand(StationCommand command);

private:
    void BuildStateMachine(void);
    void BuildTransitions(void);
    void BuildTimedTransitions(void);
    void BuildStates(void);

    //Modes
    void NormalRun();
    void TuningRun();
    void CalRun();

    //Test Actions
    void TurnOnCurrent();
    bool* CheckCurrents();


    //actions
    void StartTest();
    void CycleCurrent();
    void RunTestProbes();
    void Reset();
    //void SetStationId();
    void UpdateSerialData();

    //Heater Actions
    void RunAutoTune();
    void ToggelHeaters();

private:
    ProbeController*    probeControl;
    HeaterController*   heaterControl;
    BurnInTimer*        burnTimer;
    BurnInTimer*        testTimer;
    CommandCallback     _commandCallback=[](StationCommand){};
    Task                task,nextTask;
    Array<ProbeResult,PROBE_COUNT>  probeResults;
    Array<HeaterResult,HEATER_COUNT> heaterResults;
    bool                probeChecks[PROBE_COUNT];
    unsigned long       comInterval=COM_INTERVAL;
    unsigned long       updateInterval=UPDATE_INTERVAL;
    unsigned long       logInterval=LOG_INTERVAL;
    unsigned long       versionInterval=VER_CHECK_INTERVAL;
    SaveState           saveState;
    Timer               comTimer,updateTimer,logTimer,versionTimer;
    SerialDataOutput    comData;
    bool lockStartTest=false;
    //const char*         
    //typedef void(Controller::*ModeRun)(void);
    //ModeRun mode_run[3];
    void privateLoop() override;
};