#include "Controller.hpp"

Controller::Controller():Component(){
    //ComHandler::MapCommandCallback(this->_commandCallback);
    
}

void Controller::LoadConfigurations(){
    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig  probesConfig;
    ControllerConfig       controllerConfig;

    ConfigurationManager::LoadConfig(&heatersConfig,PacketType::HEATER_CONFIG);
    ConfigurationManager::LoadConfig(&probesConfig,PacketType::PROBE_CONFIG);
    ConfigurationManager::LoadConfig(&controllerConfig,PacketType::SYSTEM_CONFIG);

    this->heaterControl=new HeaterController(heatersConfig);
    this->probeControl=new ProbeController(probesConfig);
    this->burnTimer=new BurnInTimer(controllerConfig.burnTimerConfig);
    this->currentSelector=new CurrentSelector(controllerConfig.currentSelectConfig);
}

void Controller::SetupComponents(){
    //Send messages
    this->heaterControl->Initialize();
    this->probeControl->Initialize();
    this->currentSelector->TurnOff();
}

void Controller::HandleCommand(StationCommand command){
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