#include "HeaterController.hpp"

#pragma region Initializtion
HeaterController::HeaterController(const HeaterControllerConfig& config)
        :Component(),
        readInterval(config.readInterval),
        tempSp(config.tempSp),
        tuningWindowSize(config.windowSize),
        configuration(config){

    this->_tuningCompleteCbk=[&](HeaterTuneResult result){
        this->HeaterRunCompleteHandler(result);
    };
   for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].SetConfiguration(config.heaterConfigs[i],this->tuningWindowSize,this->tempSp);
        this->heaters[i].MapTurningComplete(this->_tuningCompleteCbk);
        RegisterChild(&this->heaters[i]);
        this->results[i]=HeaterResult();
    }
    this->mode.set(HeaterMode::HEATING,HeaterMode::HEATING);
    this->hState.set(HeaterState::HEATER_OFF,HeaterState::HEATER_OFF);
    this->tState.set(TuneState::TUNE_IDLE,TuneState::TUNE_IDLE);  
}

HeaterController::HeaterController():Component(){
    this->_tuningCompleteCbk=[&](HeaterTuneResult result){
        this->HeaterRunCompleteHandler(result);
    };
    this->_receiveWindowSizeCbk=[&](unsigned long windowSize){
        this->ReceiveWindowSizeHandler(windowSize);
    };
    
    ComHandler::MapWindowSizeCallback(this->_receiveWindowSizeCbk);

    this->modeRun[HeaterMode::HEATING]=&HeaterController::HeatingRun;
    this->modeRun[HeaterMode::ATUNE]=&HeaterController::TuneRun;

    this->tuneStateRun[TuneState::TUNE_IDLE]=&HeaterController::RunIdle;
    this->tuneStateRun[TuneState::TUNE_RUNNING]=&HeaterController::TuningRun;
    this->tuneStateRun[TuneState::TUNE_COMPLETE]=&HeaterController::RunIdle;

    this->heaterStateRun[HeaterState::HEATER_OFF]=&HeaterController::RunIdle;
    this->heaterStateRun[HeaterState::HEATER_WARMUP]=&HeaterController::RunWarmup;
    this->heaterStateRun[HeaterState::HEATER_ON]=&HeaterController::RunOn;

    this->mode.set(HeaterMode::HEATING,HeaterMode::HEATING);
    this->hState.set(HeaterState::HEATER_OFF,HeaterState::HEATER_OFF);
    this->tState.set(TuneState::TUNE_IDLE,TuneState::TUNE_IDLE);
    
}

void HeaterController::Setup(const HeaterControllerConfig& config){
    this->tempSp=config.tempSp;
    this->readInterval=config.readInterval;
    this->tuningWindowSize=config.windowSize;
    this->configuration=config;
/*     for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i]=new Heater(config.heaterConfigs[i],this->tempSp,this->tuningWindowSize);
        this->heaters[i].MapTurningComplete(this->_tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
        this->results[i]=HeaterResult();
    }   */
    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].SetConfiguration(config.heaterConfigs[i],this->tuningWindowSize,this->tempSp);
        this->heaters[i].MapTurningComplete(this->_tuningCompleteCbk);
        RegisterChild(&this->heaters[i]);
        this->results[i]=HeaterResult();
    }
    this->mode.set(HeaterMode::HEATING,HeaterMode::HEATING);
    this->hState.set(HeaterState::HEATER_OFF,HeaterState::HEATER_OFF);
    this->tState.set(TuneState::TUNE_IDLE,TuneState::TUNE_IDLE);
}

void HeaterController::Initialize(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].Initialize();
    }
    this->heaterState=HeatState::Off;
    for(uint8_t i=0;i<100;i++){
        this->ReadTemperatures();
    }
    this->tuningComTimer.onInterval([&](){
        if(this->isTuning){
            this->tuningElapsed++;
        }
        this->tuningSerialData.Set(this->results,this->tempSp,this->tuningElapsed,this->isTuning);
        ComHandler::MsgPacketSerializer(this->tuningSerialData,PacketType::TUNE_COM);
    },1000,false,false);

    this->readTimer.onInterval([&]{
        this->ReadTemperatures();
    },this->readInterval);
    RegisterChild(this->readTimer);
    RegisterChild(this->tuningComTimer);
}

void HeaterController::Run(){
    if(this->mode){
        if(this->mode.is_next(HeaterMode::ATUNE)){
            this->OnStopTuning();
        }else if(this->mode.is_next(HeaterMode::HEATING)){
            this->OnTurnOff();
        }
        this->mode.transition();
    }
    (this->*modeRun[this->mode.state])();
}

void HeaterController::HeatingRun(){
    if(this->hState){
        if(this->hState.is_next(HeaterState::HEATER_OFF)){
            this->OnTurnOff();
        }else if(this->hState.is_next(HeaterState::HEATER_WARMUP)){
            this->OnTurnOn();
        }
        this->hState.transition();
    }
    (this->*heaterStateRun[this->hState.state])();
}

void HeaterController::TuneRun(){
    if(this->tState){
        if(this->tState.is_next(TuneState::TUNE_RUNNING)){
            this->OnStartTuning();
        }else if(this->tState.is_next(TuneState::TUNE_IDLE)){
            this->OnStopTuning();
        }else if(this->tState.is_next(TuneState::TUNE_COMPLETE)){
            this->isTuning=false;
            this->tuningCompleted=true;
        }
        this->tState.transition();
    }
    (this->*tuneStateRun[this->tState.state])();
}

#pragma endregion

#pragma region Commands
void HeaterController::TurnOn(){
    if(this->mode==HeaterMode::HEATING){
        this->hState.nextState=HeaterState::HEATER_WARMUP;
    }else{
        ComHandler::SendErrorMessage(SystemError::HEATER_TRANSITION_ERR);
    }
}

void HeaterController::TurnOff(){
    if(this->mode==HeaterMode::HEATING){
        this->hState.nextState=HeaterState::HEATER_OFF;
    }else{
        ComHandler::SendErrorMessage(SystemError::HEATER_TRANSITION_ERR);
    }
}

bool HeaterController::StartTuning(){
    if(this->mode!=HeaterMode::ATUNE){
        ComHandler::SendErrorMessage(SystemError::TUNE_TRANSISITON_ERR);
        return false;
    }
    switch(this->tState.state){
        case TuneState::TUNE_IDLE:{
            this->tState.nextState=TuneState::TUNE_RUNNING;
            return true;
        }
        case TuneState::TUNE_COMPLETE:{
            ComHandler::SendErrorMessage(SystemError::TUNE_START_ERR);
            return false;
        }
        case TuneState::TUNE_RUNNING:{
            ComHandler::SendErrorMessage(SystemError::TUNE_START_ERR);
            return false;
        }
    }
}

bool HeaterController::StopTuning(){
    if(this->mode!=HeaterMode::ATUNE){
        ComHandler::SendErrorMessage(SystemError::TUNE_TRANSISITON_ERR);
        return false;
    }
    switch(this->tState.state){
        case TuneState::TUNE_IDLE:{
            ComHandler::SendErrorMessage(SystemError::TUNE_STOP_ERR);
            return false;
        }
        case TuneState::TUNE_COMPLETE:{
            ComHandler::SendErrorMessage(SystemError::TUNE_STOP_ERR);
            return false;
        }
        case TuneState::TUNE_RUNNING:{
            this->tState.nextState=TuneState::TUNE_IDLE;
            this->isTuning=false;
            return true;
        }
    }
}

void HeaterController::ToggleHeaters(){
    if(this->mode!=HeaterMode::HEATING){
        ComHandler::SendErrorMessage(SystemError::HEATER_TRANSITION_ERR);
        return;
    }
    this->hState.nextState=(this->hState.state==HeaterState::HEATER_OFF) ? 
                            HeaterState::HEATER_WARMUP:HeaterState::HEATER_OFF;
}

void HeaterController::OnTurnOn(){
    this->heaterState=HeatState::On;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOn();
    }
}

void HeaterController::OnTurnOff(){
    this->heaterState=HeatState::Off;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOff();
    }
}



bool HeaterController::SaveTuning(){
    if(this->mode!=HeaterMode::ATUNE){
        ComHandler::SendErrorMessage(SystemError::TUNE_TRANSISITON_ERR);
        return;
    }
    switch (this->tState.state){
        case TuneState::TUNE_IDLE:{
            ComHandler::SendErrorMessage(SystemError::TUNE_SAVE_CMD_ERR);
            return false;
        }
        case TuneState::TUNE_COMPLETE:{
            this->OnSaveTuning();
            this->tState.nextState=TuneState::TUNE_IDLE;
            return true;
        }
        case TuneState::TUNE_RUNNING:{
            ComHandler::SendErrorMessage(SystemError::TUNE_SAVE_CMD_ERR);
            return false;
        }
    }
}

bool HeaterController::DiscardTuning(){
    if(this->mode!=HeaterMode::ATUNE){
        ComHandler::SendErrorMessage(SystemError::TUNE_TRANSISITON_ERR);
        return;
    }
    switch (this->tState.state){
        case TuneState::TUNE_IDLE:{
            ComHandler::SendErrorMessage(SystemError::TUNE_DISCARD_CMD_ERR);
            return false;
        }
        case TuneState::TUNE_COMPLETE:{
            this->OnDiscardTuning();
            this->tState.nextState=TuneState::TUNE_IDLE;
            return true;
        }
        case TuneState::TUNE_RUNNING:{
            ComHandler::SendErrorMessage(SystemError::TUNE_DISCARD_CMD_ERR);
            return false;
        }
    }
}

void HeaterController::ReceiveWindowSizeHandler(unsigned long windowSize){
    if(this->isTuning){
        ComHandler::SendErrorMessage(SystemError::TUNE_WINDOW_FAILED);
        return;
    }
    this->tuningWindowSize=windowSize;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].SetWindowSize(windowSize);
    }
}

#pragma endregion

#pragma region StateMachineTuning

void HeaterController::HeaterRunCompleteHandler(HeaterTuneResult result){
    if(result.heaterNumber>-1){
        this->tuningResults.set(result.heaterNumber-1,result);
        bool tuningComplete=true;
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            tuningComplete&=this->tuningResults.results[i].complete;
        }
        if(result.complete){
            ComHandler::MsgPacketSerializer(result,PacketType::HEATER_NOTIFY);
        }
        if(tuningComplete){
            //Send values
            this->isTuning=false;
            this->tuningCompleted=true;
            this->tState.nextState=TuneState::TUNE_COMPLETE;
            ComHandler::MsgPacketSerializer(this->tuningResults,PacketType::HEATER_TUNE_COMPLETE);

        }
    }
}
bool HeaterController::ChangeSetPoint(uint8_t setPoint){
    if(setPoint>MAX_SETPOINT){
        ComHandler::SendErrorMessage(SystemError::MAX_TEMP_ERR,MessageType::ERROR,MAX_SETPOINT);
        return false;
    }
    this->tempSp=setPoint;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].ChangeSetpoint(setPoint);
    }
    return true;
}

void HeaterController::TuningRun(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunAutoTune();
    }
}

void HeaterController::OnStartTuning(){
    this->isTuning=true;
    this->tuningElapsed=0;
    this->tuningResults.clear();
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StartTuning();
    }
}

void HeaterController::OnStopTuning(){
    this->isTuning=false;
    this->tuningElapsed=0;
    this->tuningResults.clear();

    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StopTuning();
    }
}

void HeaterController::OnSaveTuning(){
    this->configuration.UpdateWindowSize(this->tuningWindowSize);
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        auto newPid=this->tuningResults.results[i];
        this->configuration.UpdateHeaterPid(newPid);
        this->configuration.windowSize=this->tuningWindowSize;
        this->heaters[newPid.heaterNumber-1].UpdatePid(newPid);
    }
    auto saveResult=FileManager::SaveConfiguration(&this->configuration,ConfigType::HEATER_CONFIG);
    if(saveResult){
        ComHandler::SendSystemMessage(SystemMessage::TUNING_RESULT_SAVED,MessageType::NOTIFY);
    }else{
        ComHandler::SendErrorMessage(SystemError::TUNE_SAVE_FAILED);

    } 
}

void HeaterController::OnDiscardTuning(){
    this->tuningResults.clear();
    this->tState.nextState=TuneState::TUNE_IDLE;
}

#pragma endregion

#pragma region StateMachineHeating

void HeaterController::RunOn(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunPid();
    }
}

void HeaterController::RunWarmup(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunPid();
    }
    if(this->TempOkay()){
        this->hState.nextState=HeaterState::HEATER_ON;
    }
}

void HeaterController::RunIdle(){
    _NOP();
}

#pragma endregion

#pragma region StateMachineModes

bool HeaterController::SwitchToHeating(){
    if(this->mode!=HeaterMode::HEATING){
        this->mode.nextState=HeaterMode::HEATING;
        this->hState.set(HeaterState::HEATER_OFF,HeaterState::HEATER_OFF);
        this->tuningComTimer.cancel();
        ComHandler::NotifyHeaterMode(HeaterMode::HEATING);
        return true;
    }
    return false;
}

bool HeaterController::SwitchToAutoTune(){
    if(this->mode!=HeaterMode::ATUNE){
        this->mode.nextState=HeaterMode::ATUNE;
        this->tState.set(TuneState::TUNE_IDLE,TuneState::TUNE_IDLE);
        this->tuningComTimer.start();
        ComHandler::NotifyHeaterMode(HeaterMode::ATUNE);
        return true;
    }
    return false;
}
#pragma endregion

#pragma region Misc
int HeaterController::GetSetPoint(){
    return this->heaterState==HeatState::On ? this->tempSp:0;
}

void HeaterController::ReadTemperatures(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        results[i]=heaters[i].Read();
        //results[i]=heaters[i]->GetHeaterResult();
    }
}

bool HeaterController::TempOkay(){
    bool okay=true;
    for(auto result:this->results){
        okay&=result.tempOkay;
    }
    return okay;
}

void HeaterController::GetResults(HeaterResult* fill){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
       fill[i]=this->heaters[i].GetHeaterResult();
    }
}

void HeaterController::privateLoop(){
    this->Run();
}
#pragma endregion

