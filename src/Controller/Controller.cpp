#include "Controller.hpp"

Controller::Controller():Component(){
    //ComHandler::MapCommandCallback(this->_commandCallback);
    this->_commandCallback=[&](StationCommand command){
        this->HandleCommand(command);
    };
    ComHandler::MapCommandCallback(this->_commandCallback);
    //this->mode_run[StationMode::NORMAL]=&Controller::NormalRun;
    
}

void Controller::LoadConfigurations(){
    HeaterControllerConfig heatersConfig;
    ProbeControllerConfig  probesConfig;
    ControllerConfig       controllerConfig;

    FileManager::Load(&heatersConfig,PacketType::HEATER_CONFIG);
    FileManager::Load(&probesConfig,PacketType::PROBE_CONFIG);
    FileManager::Load(&controllerConfig,PacketType::SYSTEM_CONFIG);

    this->heaterControl=new HeaterController(heatersConfig);
    this->probeControl=new ProbeController(probesConfig);
    this->burnTimer=new BurnInTimer(controllerConfig.burnTimerConfig);

    this->comInterval=controllerConfig.comInterval;
    this->updateInterval=controllerConfig.updateInterval;
    this->logInterval=controllerConfig.logInterval;
}

void Controller::SetupComponents(){
    //Send messages
    this->heaterControl->Initialize();
    this->probeControl->Initialize();
    this->probeControl->TurnOffSrc();

    this->probeResults=this->probeControl->GetProbeResults();
    this->heaterResults=this->heaterControl->GetResults();

    this->logTimer.onInterval([&](){
        if(this->burnTimer->IsRunning()){
            FileManager::Save(&this->saveState,PacketType::SAVE_STATE);
        }
    },this->logInterval);

    this->comTimer.onInterval([&](){
        this->comData.Set(this->probeResults,this->heaterResults,*(this->burnTimer));
        ComHandler::MsgPacketSerializer(this->comData,PacketType::DATA);
    },this->comInterval);

    this->updateTimer.onInterval([&](){
        this->probeResults=this->probeControl->GetProbeResults();
        this->heaterResults=this->heaterControl->GetResults();
    },this->updateInterval);

    RegisterChild(this->heaterControl);
    RegisterChild(this->probeControl);
    RegisterChild(this->logTimer);
    RegisterChild(this->comTimer);
    RegisterChild(this->updateTimer);
}

void Controller::HandleCommand(StationCommand command){
    switch(command){
        case StationCommand::START:{
            StationLogger::Log(LogLevel::INFO,true,false,"Start Command Recieved!");
            break;
        }
        case StationCommand::PAUSE:{
            StationLogger::Log(LogLevel::INFO,true,false,"Pause Command Recieved!");
            if(this->burnTimer->IsRunning()){
                this->burnTimer->Pause();
                this->nextTask=this->task;
                this->nextTask.state.n_state=States::N_PAUSED;
            }
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
        case StationCommand::CHANGE_MODE_ATUNE:{
            if(!this->burnTimer->IsRunning()){
                if(this->heaterControl->GetMode()==HeaterMode::PID_RUN){
                    this->heaterControl->ChangeMode(HeaterMode::ATUNE_RUN);
                }
                break;
            }
            StationLogger::Log(LogLevel::ERROR,true,false,SystemMessage::TEST_RUN_MODESW_ERR_MSG);
            break;
        }
        case StationCommand::CHANGE_MODE_NORMAL:{
            if(this->heaterControl->GetMode()==HeaterMode::ATUNE_RUN){
                this->heaterControl->ChangeMode(HeaterMode::PID_RUN);
                StationLogger::Log(LogLevel::INFO,true,false,SystemMessage::STATION_MODE_NORM_MSG);
                
                break;
            }
            break;
        }
        case StationCommand::START_TUNE:{
            if(this->heaterControl->GetMode()==HeaterMode::ATUNE_RUN){
                this->heaterControl->StartTuning();
                StationLogger::Log(LogLevel::INFO,true,false,SystemMessage::PID_ATUNE_START_MSG);
                break;
            }
            StationLogger::Log(LogLevel::ERROR,true,false,SystemMessage::PID_ATUNE_MODE_ERR_MSG);
            break;
        }
        case StationCommand::STOP_TUNE:{
            if(this->heaterControl->GetMode()==HeaterMode::ATUNE_RUN){
                this->heaterControl->StopTuning();
                StationLogger::Log(LogLevel::INFO,true,false,SystemMessage::PID_ATUNE_STOP_MSG);
                break;
            }
            StationLogger::Log(LogLevel::ERROR,true,false,SystemMessage::PID_ATUNE_MODE_ERR_MSG);
            break;
        }
        default:{
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,"Invalid Command Recieved");
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


bool Controller::CanTransitionTo(StationMode mode){
    if(this->task.mode!=mode){
        // switch(this->task.mode){
        //     case
        // }
    }
    return false;

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