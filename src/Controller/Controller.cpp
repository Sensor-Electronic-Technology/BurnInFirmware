#include "Controller.hpp"

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