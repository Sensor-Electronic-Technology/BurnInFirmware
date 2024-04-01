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
#include "../StressTest/TestController.hpp"
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

    //Modes
    void NormalRun();
    void TuningRun();
    void CalRun();

    //Test Actions
    bool* CheckCurrents();
    //actions
    void TestFinished();
    void RunTestProbes();
    void Reset();
    //void SetStationId();
    void UpdateSerialData();

private:
    ProbeController        probeControl;
    HeaterController       heaterControl;
    TestController         testController;
    CommandCallback        _commandCallback=[](StationCommand){_NOP();};
    TestFinsihedCallback   _testFinishedCallback=[](){_NOP();};
    Task                   task,nextTask;
    ProbeResult            probeResults[PROBE_COUNT];
    HeaterResult           heaterResults[HEATER_COUNT];
    // bool                   probeChecks[PROBE_COUNT];
    unsigned long          comInterval=COM_INTERVAL;
    unsigned long          updateInterval=UPDATE_INTERVAL;
    unsigned long          logInterval=LOG_INTERVAL;
    unsigned long          versionInterval=VER_CHECK_INTERVAL;
    SaveState              saveState;
    Timer                  comTimer,updateTimer,testTimer,versionTimer,stateLogTimer;
    SerialDataOutput       comData;
    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig  probesConfig;
    bool                    probeChecks[PROBE_COUNT]={true};
    ControllerConfig       controllerConfig;
    void privateLoop() override;
};