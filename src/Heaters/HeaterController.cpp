#include "HeaterController.hpp"

#pragma region Initializtion
HeaterController::HeaterController(const HeaterControllerConfig& config)
        :Component(),readInterval(config.readInterval),configuration(config){

    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->HeaterRunCompleteHandler(result);
    };

    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        //Heater heater(config.heaterConfigs[i]);
        this->heaters[i]=Heater(config.heaterConfigs[i]);
        // HeaterResult result;
        this->results[i]=HeaterResult();
        //this->heaters[i].SetConfiguration(config.heaterConfigs[i]);
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].MapTurningComplete(this->tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
    }
    this->modeStateMachine.Setup(&this->modes[HeaterMode::HEATING],this->modeTransitions);
    this->heatersStateMachine.Setup(&this->heaterStates[HeaterState::HEATER_OFF],this->heaterTransitions);
    this->tuningStateMachine.Setup(&this->tuneStates[TuneState::TUNE_IDLE],this->tuneTransitions);
}

HeaterController::HeaterController():Component(){
    this->tuningCompleteCbk=[&](HeaterTuneResult result){
        this->HeaterRunCompleteHandler(result);
    };
}

void HeaterController::Setup(const HeaterControllerConfig& config){
    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i]=Heater(config.heaterConfigs[i]);
        this->results[i]=HeaterResult();
    }

    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].MapTurningComplete(this->tuningCompleteCbk);
        RegisterChild(this->heaters[i]);
    }
    this->modeStateMachine.Setup(&this->modes[HeaterMode::HEATING],this->modeTransitions);
    this->heatersStateMachine.Setup(&this->heaterStates[HeaterState::HEATER_OFF],this->heaterTransitions);
    this->tuningStateMachine.Setup(&this->tuneStates[TuneState::TUNE_IDLE],this->tuneTransitions);
}

void HeaterController::Initialize(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){ 
        this->heaters[i].Initialize();
    }
    for(uint8_t i=0;i<100;i++){
        this->ReadTemperatures();
    }

    // this->printTimer.onInterval([&]{
    //     this->Print();
    // },2500);
    Serial.println(F("Heater Controller Initialize Timer Setup"));
    this->readTimer.onInterval([&]{
        this->ReadTemperatures();
    },this->readInterval);
    RegisterChild(this->readTimer);
}

#pragma endregion

#pragma region Commands
void HeaterController::TurnOn(){
    this->heaterState=HeatState::On;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOn();
    }
}

void HeaterController::TurnOff(){
    this->heaterState=HeatState::Off;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].TurnOff();
    }
}

bool HeaterController::StartTuning(){
     if(!this->tuningStateMachine.triggerEvent(TuneTrigger::TUNE_START)){
        ComHandler::SendErrorMessage(SystemError::TUNE_START_ERR);
        return false;
     }
     return true;
}

bool HeaterController::StopTuning(){
    if(!this->tuningStateMachine.triggerEvent(TuneTrigger::TUNE_STOP)){
        ComHandler::SendErrorMessage(SystemError::TUNE_STOP_ERR);
        return false;
    }
    return true;
}

void HeaterController::ToggleHeaters(){
    if(this->heaterState==HeatState::On){
        this->heatersStateMachine.triggerEvent(HeaterTrigger::START_HEATERS);
    }else{
        this->heatersStateMachine.triggerEvent(HeaterTrigger::STOP_HEATERS);
    }
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
    if(!this->CanTuningTransition()){
        return false;
    }
     if(this->tuningStateMachine.triggerEvent(TuneTrigger::TUNE_SAVED)){
        return true;
     }else{
        ComHandler::SendErrorMessage(SystemError::TUNE_SAVE_CMD_ERR);
        return false;
     }
}

bool HeaterController::DiscardTuning(){
    if(!this->CanTuningTransition()){
        return false;
    }
     if(this->tuningStateMachine.triggerEvent(TuneTrigger::TUNE_DISCARD)){
        return true;
     }else{
        ComHandler::SendErrorMessage(SystemError::TUNE_DISCARD_CMD_ERR);
        return false;
     }
}

#pragma endregion

#pragma region StateMachineTuning

bool HeaterController::CanTuningTransition(){
    if(this->modeStateMachine.GetCurrentStateId()!=HeaterMode::ATUNE){
        ComHandler::SendErrorMessage(SystemError::TUNE_TRANSISITON_ERR);
        return false;
    }
    return true;
}

void HeaterController::HeaterRunCompleteHandler(HeaterTuneResult result){
    if(result.heaterNumber<HEATER_COUNT && result.heaterNumber>-1){
        this->tuningResults.set(result.heaterNumber-1,result);
        bool tuningComplete=true;
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            tuningComplete&=this->tuningResults.results[i].complete;
        }
        if(tuningComplete){
            //Send values
            this->tuningCompleted=true;
            ComHandler::MsgPacketSerializer(this->tuningResults,PacketType::HEATER_TUNE_COMPLETE);
        }else{
            if(result.complete){
                ComHandler::MsgPacketSerializer(result,PacketType::HEATER_NOTIFY);
            }
        }
    }
}
void HeaterController::ChangeSetPoint(uint8_t setPoint){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].ChangeSetpoint(setPoint);
    }
}

void HeaterController::TuningRun(){
    this->isTuning=true;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunAutoTune();
        this->isTuning&=this->heaters[i].IsTuning();
    }
    if(!this->isTuning){
        this->tuningStateMachine.triggerEvent(TuneTrigger::TUNE_FINISHED);
    }
}

void HeaterController::OnIdleToTuning(){
    this->isTuning=true;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StartTuning();
    }
}

void HeaterController::OnAnyToIdle(){
    this->isTuning=false;
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].StopTuning();
    }
}

void HeaterController::OnCompleteToIdleSave(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        auto newPid=this->tuningResults.results[i];
        this->configuration.UpdateHeaterPid(newPid);
        this->heaters[newPid.heaterNumber-1].UpdatePid(newPid);
        auto saveResult=FileManager::SaveConfig(&this->configuration);
        if(saveResult==FileResult::SAVED){
            ComHandler::SendSystemMessage(SystemMessage::TUNING_RESULT_SAVED,MessageType::NOTIFY);
        }
    }
}

void HeaterController::OnCompleteToIdleDiscard(){
    this->tuningResults.clear();
}

#pragma endregion

#pragma region StateMachineHeating

void HeaterController::RunOn(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunPid();
    }
}

void HeaterController::WarmupRun(){
    for(uint8_t i=0;i<HEATER_COUNT;i++){
        this->heaters[i].RunPid();
    }
    if(this->TempOkay()){
        this->heatersStateMachine.triggerEvent(HeaterTrigger::TEMP_REACHED);
    }
}

bool HeaterController::CanHeatingTransition(){
    if(this->modeStateMachine.GetCurrentStateId()!=HeaterMode::HEATING){
        ComHandler::SendErrorMessage(SystemError::HEATER_TRANSITION_ERR);
        return false;
    }
    return true;
}
#pragma endregion

#pragma region StateMachineModes
bool HeaterController::SwitchToHeating(){
    return this->modeStateMachine.triggerEvent(ModeTrigger::HEATING_START);
}

bool HeaterController::SwitchToAutoTune(){
    return this->modeStateMachine.triggerEvent(ModeTrigger::ATUNE_START);
}
void HeaterController::OnHeatingToAutoTune(){
    this->TurnOff();
    this->tuningStateMachine.Reset();
}

void HeaterController::OnAutoTuneToHeating(){
    this->StopTuning();
    this->heatersStateMachine.Reset();
}
void HeaterController::RunAutoTune(){
    this->tuningStateMachine.Run();
}
void HeaterController::RunHeating(){
    this->heatersStateMachine.Run();
}
#pragma endregion

#pragma region Misc
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

void HeaterController::Print(){
    Serial.print("H[1]=");Serial.print(results[0].temperature);
    Serial.print(" , H[2]=");Serial.print(results[0].temperature);
    Serial.print(" , H[3]=");Serial.println(results[0].temperature);
}


void HeaterController::privateLoop(){
    this->modeStateMachine.Run();
}
#pragma endregion

