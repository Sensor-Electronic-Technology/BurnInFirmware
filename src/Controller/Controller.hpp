#pragma once
#include <ArduinoComponents.h>
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "CurrentSelector.hpp"
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "../Communication/ComHandler.hpp"
#include "../Logging/StationLogger.hpp"
#include "Logging/StationLogging.hpp"


#include "BurnInTimer.hpp"
#include "../constants.h"

using namespace components;

class Controller:public Component{
public:
    
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
    void HandleCommand(StationCommand command){
        switch(command){
            case StationCommand::START:{
                StationLogger::Log(LogLevel::INFO,true,false,"Start Command Recieved!");
                break;
            }
            case StationCommand::PAUSE:{
                StationLogger::Log(LogLevel::INFO,true,false,"Pause Command Recieved!");
                break;
            }
            case StationCommand::UPDATE_CONFIG:{
                StationLogger::Log(LogLevel::INFO,true,false,"Update Config Command Recieved!");
                break;
            }
            case StationCommand::PROBE_TEST:{
                StationLogger::Log(LogLevel::INFO,true,false,"Probe Test Command Recieved!");
                break;
            }
            case StationCommand::SWITCH_CURRENT:{
                StationLogger::Log(LogLevel::INFO,true,false,"Switch Current Command Recieved!");
                break;
            }
            case StationCommand::TOGGLE_HEAT:{
                StationLogger::Log(LogLevel::INFO,true,false,"Toggle Heat Command Recieved!");
                break;
            }
        }
    }

    void MapCallback(){
        ComHandler::MapCommandCallback(this->_commandCallback);
    }
private:
    ProbeController*    probeControl;
    HeaterController*   heaterControl;
    BurnInTimer*        burnTimer;
    CurrentSelector*    currentSelector; 

    CommandCallback     _commandCallback=[](StationCommand){};
    typedef void(Controller::*StateHandlers)(void);
    StateHandlers state_handler[6];
    void privateLoop() override;
};