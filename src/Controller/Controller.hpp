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
#include "../StationTImer.hpp"
#include "../constants.h"

using namespace components;


class Controller:public Component{
public:
    Controller();

    //Init
    void LoadConfigurations();
    void SetupComponents();
    void ComUpdate();
    void CheckSavedState();
    // other
    void HandleCommand(StationCommand command);
    void Acknowledge(AckType ack);
    void ConfigReceivedHandler(ConfigType,Serializable*);

private:

    //Test Actions
    bool* CheckCurrents();
    //actions
    void TestFinished();
    void Reset();
    void UpdateSerialData();
    void UpdateCurrent(int value);
    void UpdateTempSp(int value);
    void StartFromSavedState(const SaveState& savedState);


private:
    ProbeController        probeControl;
    HeaterController       heaterControl;
    TestController         testController;
    CommandCallback        _commandCallback=[](StationCommand){_NOP();};
    TestFinsihedCallback   _testFinishedCallback=[](){_NOP();};
    AckCallback            _ackCallback=[](AckType){_NOP();};
    LoadStateCallback      _loadStateCallback=[](const SaveState&){_NOP();};
    ChangeCurrentCallback  _changeCurrentCallback=[](int){_NOP();};
    ChangeTempCallback     _changeTempCallback=[](int){_NOP();};
    ConfigReceivedCallback _configReceivedCallback=[](ConfigType,Serializable*){_NOP();};
    ProbeResult            probeResults[PROBE_COUNT];
    HeaterResult           heaterResults[HEATER_COUNT];
    unsigned long          comInterval=COM_INTERVAL;
    unsigned long          updateInterval=UPDATE_INTERVAL;
    unsigned long          logInterval=LOG_INTERVAL;
    unsigned long          versionInterval=VER_CHECK_INTERVAL;
    bool toggler=false;
    SaveState              saveState;
    StationTimer           comTimer,updateTimer,testTimer,stateLogTimer,idTimer,versionTimer;
    SerialDataOutput       comData;
    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig  probesConfig;
    bool                   probeChecks[PROBE_COUNT]={true,true,true,true,true,true};
    ControllerConfig       controllerConfig;
    void privateLoop() override;
};