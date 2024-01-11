#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "../Communication/ComHandler.hpp"
#include "../Logging/StationLogger.hpp"
#include "State.hpp"
#include "CurrentSelector.hpp"
#include "BurnInTimer.hpp"
#include "../constants.h"

using namespace components;

class Controller:public Component{
public:
    
    Controller();

    //Init
    void LoadConfigurations();
    void SetupComponents();

    //actions
    void StartTest();
    void ToggelHeaters();
    void StopTest();
    void RunTestProbes();


    //States
    void RunningTest();
    void RunningProbeTest();

    //Modes
    void NormalRun();
    void TuningRun();

    //other
    void HandleCommand(StationCommand command);

    void MapCallback(){
        
    }
private:
    ProbeController*    probeControl;
    HeaterController*   heaterControl;
    BurnInTimer*        burnTimer;
    CurrentSelector*    currentSelector; 
    CommandCallback     _commandCallback=[](StationCommand){};
    Task task,nextTask;
    //typedef void(Controller::*StateHandlers)(void);
    //StateHandlers state_handler[6];
    void privateLoop() override;
};