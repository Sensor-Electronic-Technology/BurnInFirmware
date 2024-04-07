#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include <avr/wdt.h>
#include "../Serializable.hpp"
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "../Communication/ComHandler.hpp"
#include "../Communication/SerialData.hpp"
#include "../Files/FileManager.hpp"
#include "SaveState.hpp"
#include "../StressTest/TestController.hpp"
#include "../TestTimer/burn_timer_includes.h"
#include "../constants.h"

using namespace components;


class Controller:public Component{
public:
    Controller();

    //Init
    void LoadConfigurations();
    void SetupComponents();
    void CheckSavedState();

    //Transition Checks 


    //other
    void HandleCommand(StationCommand command);

private:
    void BuildStateMachine(void);

    //Modes
    void NormalRun();
    void TuningRun();
    void CalRun();

    //Transitions
    void NormalToTuning();
    void NormalToCal();
    void TuningToNormal();
    void TuningToCal();
    void CalToNormal();
    void CalToTuning();

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


    ProbeResult            probeResults[PROBE_COUNT];
    HeaterResult           heaterResults[HEATER_COUNT];
    unsigned long          comInterval=COM_INTERVAL;
    unsigned long          updateInterval=UPDATE_INTERVAL;
    unsigned long          logInterval=LOG_INTERVAL;
    unsigned long          versionInterval=VER_CHECK_INTERVAL;
    SaveState              saveState;
    Timer                  comTimer,updateTimer,testTimer,versionTimer,stateLogTimer;
    SerialDataOutput       comData;
    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig  probesConfig;
    bool                   probeChecks[PROBE_COUNT]={true,true,true,true,true,true};
    ControllerConfig       controllerConfig;
    void privateLoop() override;
};