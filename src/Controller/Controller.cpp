#include "Controller.hpp"
#include "../free_memory.h"

Controller::Controller():Component(){
    //ComHandler::MapCommandCallback(this->_commandCallback);
    this->_commandCallback=[&](StationCommand command){
        this->HandleCommand(command);
    };
    ComHandler::MapCommandCallback(this->_commandCallback);
    this->_testFinishedCallback=[&](){
        this->TestFinished();
    };

    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probeResults[i]=ProbeResult();
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaterResults[i]=HeaterResult();
    }   
    //this->mode_run[StationMode::NORMAL]=&Controller::NormalRun;
    
}

void Controller::LoadConfigurations(){
    ComHandler::SendSystemMessage(SystemMessage::BEFORE_FREE_MEM,MessageType::INIT,FreeRam());
    ComHandler::SendSystemMessage(SystemMessage::FIRMWARE_INIT,MessageType::INIT);

    ComHandler::SendSystemMessage(SystemMessage::LOAD_CONFIG,MessageType::INIT);


    FileManager::Load(&heatersConfig,PacketType::HEATER_CONFIG);
    FileManager::Load(&probesConfig,PacketType::PROBE_CONFIG);
    FileManager::Load(&controllerConfig,PacketType::SYSTEM_CONFIG);

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
        this->testController.Tick(this->probeChecks);
    },250);

    this->stateLogTimer.onInterval([&](){
        if(this->testController.IsRunning()){
            this->saveState.Set(CurrentValue::c150,85,this->testController.GetTimerData());
            FileManager::Save(&this->saveState,PacketType::SAVE_STATE);
        }
    },30000);

    this->comTimer.onInterval([&](){
        this->UpdateSerialData();
        this->comData.Set(this->probeResults,this->heaterResults,*(this->testController.GetBurnTimer()));
        Serial.print("Data: ");
        for(int i=0;i<PROBE_COUNT;i++){
            Serial.print("["+String(i)+"]:");
            Serial.print(this->comData.probeRuntimes[i]);
            Serial.print(", ");
        }
        Serial.println(" Free RAM"+String(FreeRam()));
        //ComHandler::MsgPacketSerializer(this->comData,PacketType::DATA);
    },this->comInterval);

    this->updateTimer.onInterval([&](){
        this->probeControl.GetProbeResults(this->probeResults);
        this->heaterControl.GetResults(this->heaterResults);
    },this->updateInterval);
    ComHandler::SendSystemMessage(SystemMessage::TIMER_INIT_COMPLETE,MessageType::INIT); 
    ComHandler::SendSystemMessage(SystemMessage::REG_COMPONENTS,MessageType::INIT);
    //RegisterChild(this->heaterControl);
    //RegisterChild(this->probeControl);
    RegisterChild(this->testTimer);
    RegisterChild(this->stateLogTimer);
    RegisterChild(this->comTimer);
    RegisterChild(this->updateTimer);

    RegisterChild(this->heaterControl);
    RegisterChild(this->probeControl);
    
    this->CheckSavedState();    
}

void Controller::CheckSavedState(){
    ComHandler::SendSystemMessage(SystemMessage::CHECK_SAVED_STATE,MessageType::INIT,FreeSRAM());
    FileResult result=FileManager::LoadState(&this->saveState);
    //Serial.println("FileResult: "+String(result));
    switch(result){
        case FileResult::LOADED:{
            ComHandler::SendSystemMessage(SystemMessage::STATE_LOADED,MessageType::INIT);
            if(this->testController.StartTest(this->saveState.currentTimes)){
                this->probeControl.SetCurrent(this->saveState.setCurrent);
                this->heaterControl.ChangeSetPoint(this->saveState.setTemperature);
                this->heaterControl.TurnOn();
                this->probeControl.TurnOnSrc();
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

void Controller::HandleCommand(StationCommand command){
    switch(command){
        case StationCommand::START:{
            auto tempOkay=this->heaterControl.TempOkay();
            if(tempOkay=true){//TODO: undo bypass for testing
                if(this->testController.StartTest(this->probeControl.GetSetCurrent())){
                    this->probeControl.TurnOnSrc();
                    this->heaterControl.TurnOn();
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
                this->heaterControl.SwitchToAutoTune();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::CHANGE_MODE_NORMAL:{
            if(!this->testController.IsRunning()){
                this->heaterControl.SwitchToHeating();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::START_TUNE:{
            if(!this->testController.IsRunning()){
                this->heaterControl.StartTuning();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::STOP_TUNE:{
            //TODO: Add stop tune->this only stops the tuning and puts the heaters in Tuning Idle state
            if(!this->testController.IsRunning()){
                this->heaterControl.StopTuning();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::CANCEL_TUNE:{ 
            //TODO Change to discard tune
            if(!this->testController.IsRunning()){
                this->heaterControl.DiscardTuning();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::SAVE_TUNE:{
            if(!this->testController.IsRunning()){
                this->heaterControl.SaveTuning();
            }else{
                //ComHandler::SendErrorMessage(SystemError::TEST_RUNNING,MessageType::ERROR);
            }
            break;
        }
        case StationCommand::RESET:{
            
            if(FileManager::ClearState()){
                ComHandler::SendSystemMessage(SystemMessage::SAVED_STATE_DELETED,MessageType::GENERAL);
            }else{
                ComHandler::SendErrorMessage(SystemError::STATE_DELETE_FAILED,MessageType::ERROR);
            }
            ComHandler::SendSystemMessage(SystemMessage::RESETTING_CONTROLLER,MessageType::GENERAL);
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

void Controller::Reset(){
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
}