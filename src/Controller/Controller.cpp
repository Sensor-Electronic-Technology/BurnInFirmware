#include "Controller.hpp"
#include "../free_memory.h"

Controller::Controller():Component(){
    //ComHandler::MapCommandCallback(this->_commandCallback);
    this->_commandCallback=[&](StationCommand command){
        this->HandleCommand(command);
    };
    
    this->_testFinishedCallback=[&](){
        this->TestFinished();
    };

    this->_changeCurrentCallback=[&](int value){
        this->UpdateCurrent(value);
    };

    this->_changeTempCallback=[&](int value){
        this->UpdateTempSp(value);
    };

    this->_ackCallback=[&](AckType ack){
        this->Acknowledge(ack);
    };

    ComHandler::MapAckCallback(this->_ackCallback);
    ComHandler::MapCommandCallback(this->_commandCallback);
    ComHandler::MapChangeCurrentCallback(this->_changeCurrentCallback);
    ComHandler::MapChangeTempCallback(this->_changeTempCallback);

    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probeResults[i]=ProbeResult();
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaterResults[i]=HeaterResult();
    }       
}

void Controller::LoadConfigurations(){
    ComHandler::SendSystemMessage(SystemMessage::BEFORE_FREE_MEM,MessageType::INIT,FreeRam());
    ComHandler::SendSystemMessage(SystemMessage::FIRMWARE_INIT,MessageType::INIT);

    ComHandler::SendSystemMessage(SystemMessage::LOAD_CONFIG,MessageType::INIT);


    //FileManager::Load(&heatersConfig,PacketType::HEATER_CONFIG);
    //FileManager::Load(&probesConfig,PacketType::PROBE_CONFIG);
    //FileManager::Load(&controllerConfig,PacketType::SYSTEM_CONFIG);

    // ComHandler::MsgPacketSerializer(heatersConfig,PacketType::HEATER_CONFIG);
    // ComHandler::MsgPacketSerializer(probesConfig,PacketType::PROBE_CONFIG);
    // ComHandler::MsgPacketSerializer(controllerConfig,PacketType::SYSTEM_CONFIG);
    
    this->heaterControl.Setup(heatersConfig);
    this->probeControl.Setup(probesConfig);
    this->testController.SetConfig(controllerConfig.burnTimerConfig);
    this->testController.SetFinsihedCallback(this->_testFinishedCallback);
    // this->burnTimer=new BurnInTimer(controllerConfig.burnTimerConfig);

    this->comInterval=controllerConfig.comInterval;
    this->updateInterval=controllerConfig.updateInterval;
    this->logInterval=controllerConfig.logInterval;
    this->versionInterval=controllerConfig.versionInterval;
    ComHandler::SendSystemMessage(SystemMessage::AFTER_FREE_MEM,MessageType::INIT,FreeRam());   
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
        bool probeChecks[PROBE_COUNT]={true,true,false,true,true,true};
        this->testController.Tick(probeChecks);
    },250);

    this->stateLogTimer.onInterval([&](){
        if(this->testController.IsRunning()){
            this->saveState.Set(CurrentValue::c150,85,
                                this->testController.GetTimerData(),
                                this->testController.GetTestId());
            FileManager::Save(&this->saveState,PacketType::SAVE_STATE);
        }
    },false,30000,true);
    
    this->comTimer.onInterval([&](){
        this->UpdateSerialData();
        bool probeRtOkay[PROBE_COUNT]={false,false,false,false,false,false};
        this->testController.GetProbeRunTimeOkay(probeRtOkay);
        this->comData.Set(this->probeResults,this->heaterResults,probeRtOkay,*(this->testController.GetBurnTimer()));
        this->comData.currentSP=this->probeControl.GetSetCurrent();
        this->comData.temperatureSP=this->heaterControl.GetSetPoint();
        ComHandler::MsgPacketSerializer(this->comData,PacketType::DATA);
        Serial.println(" Free RAM: "+String(FreeRam()));
    },this->comInterval,true,false);

    this->updateTimer.onInterval([&](){
        this->probeControl.GetProbeResults(this->probeResults);
        this->heaterControl.GetResults(this->heaterResults);
    },this->updateInterval);

    this->versionTimer.onInterval([&](){
        ComHandler::SendVersion();
    },VERSION_PERIOD,false,true);

    this->idTimer.onInterval([&](){
        ComHandler::SendId();
    },ID_PERIOD,false,true);

    ComHandler::SendSystemMessage(SystemMessage::TIMER_INIT_COMPLETE,MessageType::INIT); 
    ComHandler::SendSystemMessage(SystemMessage::REG_COMPONENTS,MessageType::INIT);

    RegisterChild(this->testTimer);
    RegisterChild(this->stateLogTimer);
    RegisterChild(this->comTimer);
    RegisterChild(this->updateTimer);
    RegisterChild(this->versionTimer);
    RegisterChild(this->idTimer);

    RegisterChild(this->heaterControl);
    RegisterChild(this->probeControl);
    RegisterChild(this->testController);
    
    this->CheckSavedState();    
}

void Controller::CheckSavedState(){
    ComHandler::SendSystemMessage(SystemMessage::CHECK_SAVED_STATE,MessageType::INIT,FreeSRAM());
    FileResult result=FileManager::LoadState(&this->saveState);
    switch(result){
        case FileResult::LOADED:{
            ComHandler::SendSystemMessage(SystemMessage::STATE_LOADED,MessageType::INIT);
            if(this->testController.StartTest(this->saveState.currentTimes,
                                                this->saveState.testId.c_str(),
                                                this->saveState.setCurrent,
                                                this->saveState.setTemperature)){
                                                    
                this->probeControl.SetCurrent(this->saveState.setCurrent);
                this->heaterControl.ChangeSetPoint(this->saveState.setTemperature);
                this->heaterControl.TurnOn();
                this->probeControl.TurnOnSrc();
                this->stateLogTimer.start();
            }
            break;
        }
        case FileResult::DOES_NOT_EXIST:{
            ComHandler::SendSystemMessage(SystemMessage::NO_SAVED_STATE,MessageType::INIT);
            break;
        }
        case FileResult::DESERIALIZE_FAILED:
        case FileResult::FAILED_TO_OPEN:{
            ComHandler::SendErrorMessage(SystemError::SAVED_STATE_FAILED,MessageType::ERROR);
            break;
        }
    }
    ComHandler::SendSystemMessage(SystemMessage::COMPONENTS_INIT_COMPLETE,MessageType::INIT);
    this->idTimer.start();
    this->versionTimer.start();
}

void Controller::UpdateSerialData(){
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probeResults[i].current=random(148,151);
        this->probeResults[i].voltage=random(60,65);
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaterResults[i].temperature=random(82,85);
        this->heaterResults[i].tempOkay=true;
        this->heaterResults[i].state=(bool)random(0,1);
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

void Controller::HandleCommand(StationCommand command){
    switch(command){
        case StationCommand::START:{
            auto tempOkay=this->heaterControl.TempOkay();
            if(tempOkay=true){//TODO: undo bypass for testing
/*                 if(this->testController.StartTest(this->probeControl.GetSetCurrent())){
                    this->probeControl.TurnOnSrc();
                    this->heaterControl.TurnOn();
                } */
                if(this->testController.StartTest(CurrentValue::c060)){
                    this->probeControl.TurnOnSrc();
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
            //Serial.println("Toggle Heat");
            if(!this->testController.IsRunning()){
                this->heaterControl.ToggleHeaters();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::PROBE_TEST:{
            if(!this->testController.IsRunning()){
                this->probeControl.StartProbeTest();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
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
        default:{
            ComHandler::SendErrorMessage(SystemError::INVALID_COMMAND);
            break;
        }
    }
}

void Controller::privateLoop(){

}

void Controller::UpdateCurrent(int value){
    if(!this->testController.IsRunning()){
        this->probeControl.SetCurrent((CurrentValue)value);
    }else{
       ComHandler::SendErrorMessage(SystemError::CHANGE_RUNNING_ERR,MessageType::ERROR);
    }
 
}

void Controller::UpdateTempSp(int value){
    if(!this->testController.IsRunning()){
        this->heaterControl.ChangeSetPoint(value);
    }else{

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
}