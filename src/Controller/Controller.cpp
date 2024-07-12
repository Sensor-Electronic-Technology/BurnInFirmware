#include "Controller.hpp"
#include "../free_memory.h"

Controller::Controller():Component(){
    this->_commandCallback=[&](StationCommand command){
        this->HandleCommand(command);
    };
    
    this->_testFinishedCallback=[&](){
        this->TestFinished();
    };

    this->_ackCallback=[&](AckType ack){
        this->Acknowledge(ack);
    };

    this->_loadStateCallback=[&](const SaveState& state){
        this->StartFromSavedState(state);
    };

    this->_getConfigCallback=[&](ConfigType configType){
        this->GetConfigHandler(configType);
    };

    this->_restartRequiredCallback=[&](){
        this->NeedRestartHandler();
    };

    ComHandler::MapAckCallback(this->_ackCallback);
    ComHandler::MapCommandCallback(this->_commandCallback);
    ComHandler::MapRestartCallback(this->_restartRequiredCallback);
    ComHandler::MapGetConfigCallback(this->_getConfigCallback);

    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probeResults[i]=ProbeResult();
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaterResults[i]=HeaterResult();
    }       
}

void Controller::LoadConfigurations(){
    ComHandler::SendSystemMessage(SystemMessage::BEFORE_FREE_MEM,MessageType::INIT,FreeSRAM());
    ComHandler::SendSystemMessage(SystemMessage::FIRMWARE_INIT,MessageType::INIT);
    ComHandler::SendSystemMessage(SystemMessage::LOAD_CONFIG,MessageType::INIT);

    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig probesConfig;
    ControllerConfig controllerConfig;
    
/*    FileManager::SaveConfiguration(&heatersConfig,ConfigType::HEATER_CONFIG);
    FileManager::SaveConfiguration(&probesConfig,ConfigType::PROBE_CONFIG);
    FileManager::SaveConfiguration(&controllerConfig,ConfigType::SYSTEM_CONFIG);   */

    if(!FileManager::LoadConfiguration(&heatersConfig,ConfigType::HEATER_CONFIG)){
       heatersConfig.Reset();
    }
    if(!FileManager::LoadConfiguration(&probesConfig,ConfigType::PROBE_CONFIG)){
        probesConfig.Reset();
    }

    if(!FileManager::LoadConfiguration(&controllerConfig,ConfigType::SYSTEM_CONFIG)){
        controllerConfig.Reset();
    }

    this->heaterControl.Setup(heatersConfig);
    this->probeControl.Setup(probesConfig);
    this->testController.SetConfig(controllerConfig.burnTimerConfig);
    this->testController.SetFinsihedCallback(this->_testFinishedCallback);

    this->comInterval=controllerConfig.comInterval;
    this->updateInterval=controllerConfig.updateInterval;
    this->logInterval=controllerConfig.logInterval;
    this->versionInterval=controllerConfig.versionInterval;
    ComHandler::SendSystemMessage(SystemMessage::AFTER_FREE_MEM,MessageType::INIT,FreeSRAM());   
    ComHandler::SendSystemMessage(SystemMessage::CONFIG_LOADED,MessageType::INIT);
}

void Controller::SetupComponents(){
    ComHandler::SendSystemMessage(SystemMessage::COMPONENT_INIT,MessageType::INIT);

    this->probeControl.Initialize();
    this->probeControl.TurnOffSrc();
    this->probeControl.GetProbeResults(this->probeResults);
    ComHandler::SendSystemMessage(SystemMessage::PROBES_INIT,MessageType::INIT);
    
    this->heaterControl.Initialize();
    this->heaterControl.GetResults(this->heaterResults);

    ComHandler::SendSystemMessage(SystemMessage::HEATERS_INIT,MessageType::INIT);

    ComHandler::SendSystemMessage(SystemMessage::TIMER_INIT,MessageType::INIT); 

    this->testTimer.onInterval([&](){
/*         bool probeChecks[PROBE_COUNT]={false};
        this->UpdateSerialData(); */
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            probeChecks[i]=this->probeResults[i].okay;
        }
        this->testController.Tick(probeChecks);
    },250);

    this->stateLogTimer.onInterval([&](){
        if(this->testController.IsRunning()){
            this->saveState.Set(CurrentValue::c150,85,
                                this->testController.GetTimerData(),
                                this->testController.GetTestId());
            FileManager::SaveState(&this->saveState);
        }
    },10000,false,true);

    this->comTimer.onInterval([&](){ 
         this->ComUpdate();
    }, this->comInterval, true, false);

    this->updateTimer.onInterval([&](){
        this->probeControl.GetProbeResults(this->probeResults);
        this->heaterControl.GetResults(this->heaterResults);
        //this->UpdateSerialData();
    },this->updateInterval,true,false);

/*     this->versionTimer.onInterval([&](){
        ComHandler::SendVersion();
    },VERSION_PERIOD,false,true); */

/*     this->idTimer.onInterval([&](){
        ComHandler::SendId();
    },ID_PERIOD,false,true); */

    ComHandler::SendSystemMessage(SystemMessage::TIMER_INIT_COMPLETE,MessageType::INIT); 
    ComHandler::SendSystemMessage(SystemMessage::REG_COMPONENTS,MessageType::INIT);

    RegisterChild(this->testTimer);
    RegisterChild(this->stateLogTimer);
    RegisterChild(this->comTimer);
    RegisterChild(this->updateTimer);

    RegisterChild(this->heaterControl);
    RegisterChild(this->probeControl);
    RegisterChild(this->testController);
    
    this->CheckSavedState(0);
}

void Controller::ComUpdate(){
/*     this->probeControl.GetProbeResults(this->probeResults);
    this->heaterControl.GetResults(this->heaterResults); */
    bool probeRtOkay[PROBE_COUNT] = {false, false, false, false, false, false}; 
    this->testController.GetProbeRunTimeOkay(probeRtOkay);
    this->comData.Set(this->probeResults, this->heaterResults, probeRtOkay, *(this->testController.GetBurnTimer()));
    this->comData.currentSP = this->probeControl.GetSetCurrent();
    this->comData.temperatureSP = this->heaterControl.GetSetPoint();
    ComHandler::MsgPacketSerializer(this->comData, PacketType::DATA);
}

void Controller::NeedRestartHandler(){
    if(this->testController.IsRunning() || this->heaterControl.IsTuning()){
        this->needsReset=true;
        return;
    }
    this->Reset();
}

void Controller::GetConfigHandler(ConfigType configType){
    switch(configType){
        case ConfigType::HEATER_CONFIG:{
            HeaterControllerConfig config;
            auto success=FileManager::LoadConfiguration(&config,configType);
            if(success){
                ComHandler::SendConfig(configType,&config);
            }else{
                ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED);
            }
            break;
        }
        case ConfigType::PROBE_CONFIG:{
            ProbeControllerConfig config;
            auto success=FileManager::LoadConfiguration(&config,configType);
            if(success){
                ComHandler::SendConfig(configType,&config);
            }else{
                ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED);
            }
            break;
        }
        case ConfigType::SYSTEM_CONFIG:{
            ControllerConfig config;
            auto success=FileManager::LoadConfiguration(&config,configType);
            if(success){
                ComHandler::SendConfig(configType,&config);
            }else{
                ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED);
            }
            break;
        }
        default:{
            ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED);
            break;
        }
    }
}

void Controller::FormatSdHandler(){
    FileManager::FormatNoBackup();
}

void Controller::CheckSavedState(int attempts){
    ComHandler::SendSystemMessage(SystemMessage::CHECK_SAVED_STATE,MessageType::INIT,FreeSRAM());
    SaveState loadState;
    FileResult result=FileManager::LoadState(&loadState);
    switch(result){
        case FileResult::LOADED:{
            ComHandler::SendSystemMessage(SystemMessage::STATE_LOADED,MessageType::INIT);
            this->StartFromSavedState(loadState);
            break;
        }
        case FileResult::DOES_NOT_EXIST:{
            ComHandler::SendSystemMessage(SystemMessage::NO_SAVED_STATE,MessageType::INIT);
            break;
        }
        case FileResult::DESERIALIZE_FAILED:
        case FileResult::FAILED_TO_OPEN:{
            if(attempts>2){
                ComHandler::SendErrorMessage(SystemError::SAVED_STATE_FAILED,MessageType::ERROR);
                break;
            }
            FileManager::ClearState();
            this->CheckSavedState(attempts+1);
            break;
        }
    }
    ComHandler::SendSystemMessage(SystemMessage::COMPONENTS_INIT_COMPLETE,MessageType::INIT);
}

void Controller::UpdateSerialData(){
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        if(i==2){
            if(this->testController.GetBurnTimer()->GetElapsed()>30){
                this->probeResults[i].current=0;
                this->probeResults[i].voltage=random(60,65);
            }else{
                this->probeResults[i].current=random(148,151);
                this->probeResults[i].voltage=random(60,65);
            }
        
        }else{
            this->probeResults[i].current=random(148,151);
            this->probeResults[i].voltage=random(60,65);
        }
       
        this->probeResults[i].check(93.5,this->probeControl.GetSetCurrent());
    }
    /* this->heaterControl.GetResults(this->heaterResults); */
    /* this->probeControl.GetProbeResults(this->probeResults); */

    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaterResults[i].temperature=random(82,85);
        this->heaterResults[i].tempOkay=true;
        this->heaterResults[i].state=!this->heaterResults[i].state;
    }
}

void Controller::Acknowledge(AckType ack){
    switch(ack){
        case AckType::TEST_START_ACK:{
            this->testController.AcknowledgeTestStart();
            break;
        }
        case AckType::VER_ACK:{
            this->versionTimer.cancel();
            break;
        }
        case AckType::ID_ACK:{
            this->idTimer.cancel();
            break;
        }
        case AckType::TEST_FINISH_ACK:{
            this->testController.AcknowledgeTestComplete();
            break;
        }
    }
}

void Controller::StartFromSavedState(const SaveState& savedState){
    if(this->testController.IsRunning()){
        ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR,MessageType::ERROR);
        return;
    }
    if(this->testController.StartTest(savedState)){
        this->saveState=savedState;
        this->probeControl.SetCurrent(this->saveState.setCurrent);
        this->heaterControl.ChangeSetPoint(this->saveState.setTemperature);
        this->heaterControl.TurnOn();
        this->probeControl.TurnOnSrc();
        this->stateLogTimer.start();
    }
}

void Controller::HandleCommand(StationCommand command){
    switch(command){
        case StationCommand::START:{
            auto tempOkay=this->heaterControl.TempOkay();
            if(tempOkay=true){//TODO: undo bypass for testing
                if(this->testController.StartTest(this->probeControl.GetSetCurrent())){
                        this->probeControl.TurnOnSrc();
                        this->heaterControl.TurnOn();
                        this->stateLogTimer.start();
                } 
            }else{
                //TODO: Send temperature notification
            }
            break;
        }
        case StationCommand::PAUSE:{
            if(this->testController.PauseTest()){
                this->probeControl.TurnOffSrc();
            }
            break;
        }

        case StationCommand::CONTINUE:{
            if(this->testController.ContinueTest()){
                this->probeControl.TurnOnSrc();
            }
            break;
        }
        case StationCommand::TOGGLE_HEAT:{
            if(!this->testController.IsRunning()){
                this->heaterControl.ToggleHeaters();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::PROBE_TEST:{
            if(!this->testController.IsRunning()){
                this->probeControl.StartProbeTest();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::CYCLE_CURRENT:{
            if(!this->testController.IsRunning()){
                this->probeControl.CycleCurrent();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::CHANGE_MODE_ATUNE:{
            if(!this->testController.IsRunning()){
                if(this->heaterControl.SwitchToAutoTune()){
                    this->comTimer.cancel();
                }
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::CHANGE_MODE_NORMAL:{
            if(!this->testController.IsRunning()){
                if(this->heaterControl.SwitchToHeating()){
                    this->comTimer.start();
                }
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::START_TUNE:{
            if(!this->testController.IsRunning()){
                this->heaterControl.StartTuning();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::STOP_TUNE:{
            if(!this->testController.IsRunning()){
                this->heaterControl.StopTuning();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::CANCEL_TUNE:{ 
            //TODO Change to discard tune
            if(!this->testController.IsRunning()){
                this->heaterControl.DiscardTuning();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::SAVE_TUNE:{
            if(!this->testController.IsRunning()){
                this->heaterControl.SaveTuning();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        case StationCommand::RESET:{
            this->Reset();
            break;
        }
        case StationCommand::REQUEST_RUNNING_TEST:{
            this->testController.SendRunningTest();
            break;
        }
        case StationCommand::FORMAT_SD:{
            if(!this->testController.IsRunning() && !this->heaterControl.IsTuning()){
                this->FormatSdHandler();
                this->Reset();
            }else{
                ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
            }
            break;
        }
        default:{
            ComHandler::SendErrorMessage(SystemError::INVALID_COMMAND);
            break;
        }
    }
}

void Controller::privateLoop(){

}

void Controller::UpdateTempSp(int value){
    if(this->testController.IsRunning()){
        ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR,MessageType::ERROR);
        return;
    }
    if(this->heaterControl.ChangeSetPoint(value)){
        HeaterControllerConfig heatersConfig;
        FileManager::LoadConfiguration(&heatersConfig,ConfigType::HEATER_CONFIG);
        heatersConfig.tempSp=value;
        FileManager::SaveConfiguration(&heatersConfig,ConfigType::HEATER_CONFIG);
    }
}

void Controller::Reset(){
    if(FileManager::ClearState()){
        ComHandler::SendSystemMessage(SystemMessage::SAVED_STATE_DELETED,MessageType::GENERAL);
    }else{
        ComHandler::SendErrorMessage(SystemError::STATE_DELETE_FAILED,MessageType::ERROR);
    }
    ComHandler::SendSystemMessage(SystemMessage::RESETTING_CONTROLLER,MessageType::GENERAL);
    wdt_disable();
	wdt_enable(WDTO_15MS);
	while(true){_NOP();}
}

bool* Controller::CheckCurrents(){
    bool checks[PROBE_COUNT]={false};
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        checks[i]=this->probeResults[i].okay;
    }
    return checks;
}

void Controller::TestFinished(){
    this->probeControl.TurnOffSrc();
    this->heaterControl.TurnOff();
    this->saveState.Clear();
    FileManager::ClearState();
    if(this->needsReset){
        ComHandler::SendSystemMessage(SystemMessage::RESETTING_CONTROLLER,MessageType::GENERAL);
        this->Reset();
    }
}