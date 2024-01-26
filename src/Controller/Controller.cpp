#include "Controller.hpp"
#include "../free_memory.h"

Controller::Controller():Component(){
    //ComHandler::MapCommandCallback(this->_commandCallback);
    this->_commandCallback=[&](StationCommand command){
        this->HandleCommand(command);
    };
    ComHandler::MapCommandCallback(this->_commandCallback);
    //this->mode_run[StationMode::NORMAL]=&Controller::NormalRun;
    
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
    this->burnTimer=new BurnInTimer(controllerConfig.burnTimerConfig);

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
    this->logTimer.onInterval([&](){
        if(this->burnTimer->IsRunning()){
            this->saveState.Set(CurrentValue::c150,85,this->burnTimer->testTimer);
            //FileManager::Save(&this->saveState,PacketType::SAVE_STATE);
        }
        this->UpdateSerialData();
        this->comData.Set(this->probeResults,this->heaterResults,*(this->burnTimer));
        ComHandler::MsgPacketSerializer(this->comData,PacketType::DATA);
    },1000);

    this->comTimer.onInterval([&](){
        this->comData.Set(this->probeResults,this->heaterResults,*(this->burnTimer));
        //ComHandler::MsgPacketSerializer(this->comData,PacketType::DATA);
    },this->comInterval);

    this->updateTimer.onInterval([&](){
        this->probeResults=this->probeControl->GetProbeResults();
        this->heaterResults=this->heaterControl->GetResults();
    },this->updateInterval);

    StationLogger::LogInit(LogLevel::INFO,true,F("Registering Components..."));
    //RegisterChild(this->heaterControl);
    //RegisterChild(this->probeControl);
    RegisterChild(this->logTimer);
    //RegisterChild(this->comTimer);
    //RegisterChild(this->updateTimer);
    StationLogger::LogInit(LogLevel::INFO,true,F("Checking for saved state"));
    StationLogger::LogInit(LogLevel::INFO,true,F("Free Memory: %d"),FreeSRAM());
    this->CheckSavedState();
    StationLogger::LogInit(LogLevel::INFO,true,F("-------Initalization Complete-------"));
    
}

void Controller::CheckSavedState(){
    // FileManager::Load(&this->saveState,PacketType::SAVE_STATE);
    // if(this->saveState.currentTimes.running){
    //     this->burnTimer->StartFrom(this->saveState.currentTimes);
    // }
    FileResult result=FileManager::LoadState(&this->saveState);
    switch(result){
        case FileResult::LOADED:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Saved State Found! Continuing Test"));
            this->burnTimer->StartFrom(this->saveState.currentTimes);
            // this->heaterControl->TurnOn();
            // this->TurnOnCurrent();
            break;
        }
        case FileResult::DOES_NOT_EXIST:{
            StationLogger::Log(LogLevel::INFO,true,false,F("No Saved State found, continuing normal operation"));
            this->lockStartTest=false;
            break;
        }
        case FileResult::DESERIALIZE_FAILED:
        case FileResult::FAILED_TO_OPEN:{
            this->lockStartTest=true;
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
            StationLogger::Log(LogLevel::INFO,true,false,F("Start Command Recieved"));

            if(!this->burnTimer->IsRunning()){
                this->burnTimer->Start(CurrentValue::c150);
                StationLogger::Log(LogLevel::INFO,true,false,F("BurnTimer started"));
            }else{
                if(this->burnTimer->IsPaused()){
                    this->burnTimer->Continue();
                    StationLogger::Log(LogLevel::INFO,true,false,F("BurnTimer continuing"));
                }else{
                    StationLogger::Log(LogLevel::INFO,true,false,F("Station is running and not paused"));
                }
            }
            break;
        }
        case StationCommand::PAUSE:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Pause Command Recieved"));
            if(!this->burnTimer->IsPaused()){
                this->burnTimer->Pause();
                StationLogger::Log(LogLevel::INFO,true,false,F("BurnTimer Paused"));
                // this->nextTask=this->task;
                // this->nextTask.state.n_state=States::N_PAUSED;
            }else{
                StationLogger::Log(LogLevel::INFO,true,false,F("BurnTimer already paused,send Start command to continue"));
            }
            break;
        }
        case StationCommand::PROBE_TEST:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Probe Test Command Recieved!"));
            break;
        }
        case StationCommand::CYCLE_CURRENT:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Switch Current Command Recieved!"));
            break;
        }
        case StationCommand::TOGGLE_HEAT:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Toggle Heat Command Recieved!"));
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
    switch(this->task.mode){
        case StationMode::NORMAL:{
            this->NormalRun();
            break;
        }
        case StationMode::TUNE:{
            this->TuningRun();
            break;
        }
        case StationMode::CAL:{
            this->CalRun();
            break;
        }
    }
}

void Controller::privateLoop(){
    if(this->burnTimer->IsRunning()){
        bool checks[PROBE_COUNT]={true};
        this->burnTimer->Increment(checks);
        
    }
}

void Controller::Reset(){
    wdt_disable();
	wdt_enable(WDTO_15MS);
	while(true){;}
}

void Controller::CycleCurrent(){
    this->probeControl->CycleCurrent();
}

void Controller::TurnOnCurrent(){
    this->probeControl->TurnOnSrc();
}

bool* Controller::CheckCurrents(){
    bool checks[PROBE_COUNT]={false};
    for(int i=0;i<PROBE_COUNT;i++){
        checks[i]=this->probeResults[i].okay;
    }
    return checks;
}

void Controller::TuningRun(){

}
void Controller::CalRun(){
    
}


void Controller::NormalRun(){
    if(!this->task.m_latched){

        //others
        this->task.m_latched=true;
    }
    switch(this->task.state.n_state){
        case States::N_INIT:{
            this->heaterControl->TurnOff();
            this->probeControl->TurnOffSrc();
            this->task.state.n_state=States::N_IDLE;
            break;
        }
        case States::N_RUNNING:{
            if(!this->task.s_latched){
                this->task.s_latched=true;
            }
            auto currentChecks=this->CheckCurrents();
            this->burnTimer->Increment(currentChecks);
            if(this->burnTimer->IsDone()){
                this->probeControl->TurnOffSrc();
                this->heaterControl->TurnOff();
                this->task.state.n_state=States::N_DONE;
            }
            break; 
        }
        case States::N_PAUSED:{
            if(!this->task.s_latched){
                this->probeControl->TurnOffSrc();
                this->task.s_latched=true;
            }
            if(!this->burnTimer->IsPaused()){
                this->burnTimer->Continue();
                this->probeControl->TurnOffSrc();
                this->task.s_latched=true;
                this->task.state.n_state=States::N_RUNNING;
            }
            //wait,no log
            break; 
        }

        case States::N_IDLE:{
            //wait
            break; 
        }
        case States::N_DONE:{
            //Erase logs
            break; 
        }
        case States::N_ERROR:{
            //delete?
            break; 
        }
    }

}