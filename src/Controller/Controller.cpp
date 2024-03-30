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
    //this->mode_run[StationMode::NORMAL]=&Controller::NormalRun;
    
}

void Controller::BuildStateMachine(void){

}

void Controller::LoadConfigurations(){
    StationLogger::LogInit(LogLevel::INFO,true,F("--------Firmware Initialization Starting--------"));
    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig  probesConfig;
    ControllerConfig       controllerConfig;
    StationLogger::LogInit(LogLevel::INFO,true,F("Loading configuration files..."));

    FileManager::Load(&heatersConfig,PacketType::HEATER_CONFIG);
    FileManager::Load(&probesConfig,PacketType::PROBE_CONFIG);
    FileManager::Load(&controllerConfig,PacketType::SYSTEM_CONFIG);


    // ComHandler::MsgPacketSerializer(heatersConfig,PacketType::HEATER_CONFIG);
    // ComHandler::MsgPacketSerializer(probesConfig,PacketType::PROBE_CONFIG);
    // ComHandler::MsgPacketSerializer(controllerConfig,PacketType::SYSTEM_CONFIG);
    
    this->heaterControl=new HeaterController(heatersConfig);
    this->probeControl=new ProbeController(probesConfig);
    this->testController=new TestController(controllerConfig.burnTimerConfig);
    this->testController->SetFinsihedCallback(this->_testFinishedCallback);
    // this->burnTimer=new BurnInTimer(controllerConfig.burnTimerConfig);

    this->comInterval=controllerConfig.comInterval;
    this->updateInterval=controllerConfig.updateInterval;
    this->logInterval=controllerConfig.logInterval;
    this->versionInterval=controllerConfig.versionInterval;
    StationLogger::LogInit(LogLevel::INFO,true,F("--------Configuration Files Loaded--------"));
}

void Controller::SetupComponents(){
    StationLogger::LogInit(LogLevel::INFO,true,F("-------Initalizing Components-------"));
    //Send messages

    this->heaterControl->Initialize();
    this->probeControl->Initialize();
    this->probeControl->TurnOffSrc();

    StationLogger::LogInit(LogLevel::INFO,true,F("Reading intitial values..."));
    this->probeResults=this->probeControl->GetProbeResults();
    this->heaterResults=this->heaterControl->GetResults();

    StationLogger::LogInit(LogLevel::INFO,true,F("Setting up timers..."));

    this->testTimer.onInterval([&](){
        bool probeChecks[PROBE_COUNT]={true};
        this->testController->Tick(probeChecks);
    },250);

    this->stateLogTimer.onInterval([&](){
        if(this->testController->IsRunning()){
            this->saveState.Set(CurrentValue::c150,85,this->testController->GetTimerData());
            FileManager::Save(&this->saveState,PacketType::SAVE_STATE);
        }
    },30000);

    this->comTimer.onInterval([&](){
        this->UpdateSerialData();
        this->comData.Set(this->probeResults,this->heaterResults,*(this->testController->GetBurnTimer()));
        ComHandler::MsgPacketSerializer(this->comData,PacketType::DATA);
    },this->comInterval);

    this->updateTimer.onInterval([&](){
        this->probeResults=this->probeControl->GetProbeResults();
        this->heaterResults=this->heaterControl->GetResults();
    },this->updateInterval);

    StationLogger::LogInit(LogLevel::INFO,true,F("Registering Components..."));
    //RegisterChild(this->heaterControl);
    //RegisterChild(this->probeControl);
    RegisterChild(this->testTimer);
    //RegisterChild(this->comTimer);
    //RegisterChild(this->updateTimer);
    StationLogger::LogInit(LogLevel::INFO,true,F("Checking for saved state"));
    StationLogger::LogInit(LogLevel::INFO,true,F("Free Memory: %d"),FreeSRAM());
    this->CheckSavedState();
    StationLogger::LogInit(LogLevel::INFO,true,F("-------Initalization Complete-------"));
    
}

void Controller::CheckSavedState(){
    FileResult result=FileManager::LoadState(&this->saveState);
    switch(result){
        case FileResult::LOADED:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Saved State Found! Continuing Test"));
            if(this->testController->StartTest(this->saveState.currentTimes)){
                this->probeControl->SetCurrent(this->saveState.setCurrent);
                this->heaterControl->ChangeSetPoint(this->saveState.setTemperature);
                this->heaterControl->TurnOn();
                this->probeControl->TurnOnSrc();
            }
            break;
        }
        case FileResult::DOES_NOT_EXIST:{
            StationLogger::Log(LogLevel::INFO,true,false,F("No Saved State found, continuing normal operation"));
            break;
        }
        case FileResult::DESERIALIZE_FAILED:
        case FileResult::FAILED_TO_OPEN:{
            StationLogger::Log(LogLevel::ERROR,true,false,F("Failed to load saved state"));
            break;
        }
    }
}

void Controller::UpdateSerialData(){
    for(int i=0;i<PROBE_COUNT;i++){
        this->probeResults[i].current=random(148,151);
        this->probeResults[i].voltage=random(60,65);
    }
    for(int i=0;i<HEATER_COUNT;i++){
        this->heaterResults[i].temperature=random(82,85);
        this->heaterResults[i].tempOkay=true;
        this->heaterResults[i].state=(bool)random(0,1);
    }
}

void Controller::HandleCommand(StationCommand command){
    switch(command){
        case StationCommand::START:{
            auto tempOkay=this->heaterControl->TempOkay();
            if(tempOkay=true){//TODO: undo bypass for testing
                if(this->testController->StartTest(this->probeControl->GetSetCurrent())){
                    this->probeControl->TurnOnSrc();
                    this->heaterControl->TurnOn();
                }
            }
            break;
        }
        case StationCommand::PAUSE:{
            if(this->testController->PauseTest()){
                this->probeControl->TurnOffSrc();
            }
            break;
        }
        case StationCommand::PROBE_TEST:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Probe Test Command Recieved!"));
            break;
        }
        case StationCommand::CYCLE_CURRENT:{
            if(!this->testController->IsRunning()){
                this->probeControl->CycleCurrent();
            }
            break;
        }
        case StationCommand::TOGGLE_HEAT:{
            if(!this->testController->IsRunning()){
                this->heaterControl->ToggleHeaters();
            }
            break;
        }
        case StationCommand::CHANGE_MODE_ATUNE:{
            // if(!this->burnTimer->IsRunning()){
            //     if(this->heaterControl->GetMode()==HeaterMode::PID_RUN){
            //         this->heaterControl->ChangeMode(HeaterMode::ATUNE_RUN);
            //     }
            //     break;
            // }
            StationLogger::Log(LogLevel::ERROR,true,false,SystemMessage::TEST_RUN_MODESW_ERR_MSG);
            break;
        }
        case StationCommand::CHANGE_MODE_NORMAL:{
            // if(this->heaterControl->GetMode()==HeaterMode::ATUNE_RUN){
            //     this->heaterControl->ChangeMode(HeaterMode::PID_RUN);
            //     StationLogger::Log(LogLevel::INFO,true,false,SystemMessage::STATION_MODE_NORM_MSG);
                
            //     break;
            // }
            break;
        }
        case StationCommand::START_TUNE:{
            // if(this->heaterControl->GetMode()==HeaterMode::ATUNE_RUN){
            //     this->heaterControl->StartTuning();
            //     StationLogger::Log(LogLevel::INFO,true,false,SystemMessage::PID_ATUNE_START_MSG);
            //     break;
            // }
            StationLogger::Log(LogLevel::ERROR,true,false,SystemMessage::PID_ATUNE_MODE_ERR_MSG);
            break;
        }
        case StationCommand::STOP_TUNE:{
            // if(this->heaterControl->GetMode()==HeaterMode::ATUNE_RUN){
            //     this->heaterControl->StopTuning();
            //     StationLogger::Log(LogLevel::INFO,true,false,SystemMessage::PID_ATUNE_STOP_MSG);
            //     break;
            // }
            StationLogger::Log(LogLevel::ERROR,true,false,SystemMessage::PID_ATUNE_MODE_ERR_MSG);
            break;
        }
        case StationCommand::RESET:{
            StationLogger::Log(LogLevel::WARNING,true,false,F("Deleting saved state"));
            if(FileManager::ClearState()){
                StationLogger::Log(LogLevel::INFO,true,false,F("Saved State Deleted"));
            }else{
                StationLogger::Log(LogLevel::WARNING,true,false,F("Failed to clear saved state"));
            }
            StationLogger::Log(LogLevel::WARNING,true,false,F("Controller Resetting, please wait..."));
            this->Reset();
            break;
        }
        default:{
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Invalid Command Recieved"));
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
    for(int i=0;i<PROBE_COUNT;i++){
        checks[i]=this->probeResults[i].okay;
    }
    return checks;
}

void Controller::TestFinished(){
    this->probeControl->TurnOffSrc();
    this->heaterControl->TurnOff();

}

void Controller::TuningRun(){

}

void Controller::NormalRun(){
    if(!this->task.m_latched){

        //others
        this->task.m_latched=true;
    }
    switch(this->task.state.n_state){
        case Station_States::N_INIT:{
            this->heaterControl->TurnOff();
            this->probeControl->TurnOffSrc();
            this->task.state.n_state=Station_States::N_IDLE;
            break;
        }
        case Station_States::N_RUNNING:{
            if(!this->task.s_latched){
                this->task.s_latched=true;
            }
            auto currentChecks=this->CheckCurrents();
            this->burnTimer->Increment(currentChecks);
            if(this->burnTimer->IsDone()){
                this->probeControl->TurnOffSrc();
                this->heaterControl->TurnOff();
                this->task.state.n_state=Station_States::N_DONE;
            }
            break; 
        }
        case Station_States::N_PAUSED:{
            if(!this->task.s_latched){
                this->probeControl->TurnOffSrc();
                this->task.s_latched=true;
            }
            if(!this->burnTimer->IsPaused()){
                this->burnTimer->Continue();
                this->probeControl->TurnOffSrc();
                this->task.s_latched=true;
                this->task.state.n_state=Station_States::N_RUNNING;
            }
            //wait,no log
            break; 
        }

        case Station_States::N_IDLE:{
            //wait
            break; 
        }
        case Station_States::N_DONE:{
            //Erase logs
            break; 
        }
        case Station_States::N_ERROR:{
            //delete?
            break; 
        }
    }

}