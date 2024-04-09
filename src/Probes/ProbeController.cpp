#include "ProbeController.hpp"

ProbeController::ProbeController(const ProbeControllerConfig& config)
    :Component(),
    currentSelector(config.currentSelectConfig),
    testCurrent(config.probeTestCurrent),
    currentPercent(config.probeCurrentPercent),
    probeTestTime(config.probeTestTime){

    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probes[i]=new Probe(config.probeConfigs[i]);
        RegisterChild(this->probes[i]);
        this->results[i]=ProbeResult();
    }
    
    this->readTimer.onInterval([&]{
        this->Read();
    },config.readInterval);

    this->probeTestTimer.setTimeout([&]{
        ComHandler::SendSystemMessage(SystemMessage::PROBE_TEST_END,MessageType::NOTIFY);
        this->TurnOffSrc();
        this->currentSelector.SetCurrent(this->savedCurrent);
    },this->probeTestTime);

    RegisterChild(this->readTimer);
    RegisterChild(this->probeTestTimer);
}

ProbeController::ProbeController():Component(){

}

void ProbeController::Setup(const ProbeControllerConfig& config){
    this->currentSelector.Setup(config.currentSelectConfig);
    this->testCurrent=config.probeTestCurrent;
    this->currentPercent=config.probeCurrentPercent;
    this->probeTestTime=config.probeTestTime;
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probes[i]=new Probe(config.probeConfigs[i]);
        RegisterChild(this->probes[i]);
        this->results[i]=ProbeResult();
    }
    
    this->readTimer.onInterval([&]{
        this->Read();
    },config.readInterval);

    this->probeTestTimer.setTimeout([&]{
        this->TurnOffSrc();
        this->currentSelector.SetCurrent(this->savedCurrent);
        ComHandler::SendSystemMessage(SystemMessage::PROBE_TEST_END,
                MessageType::NOTIFY,
                (uint8_t)this->currentSelector.GetSetCurrent());
    },this->probeTestTime);
    RegisterChild(this->readTimer);
    RegisterChild(this->probeTestTimer);
}

void ProbeController::Initialize(){
    this->currentSelector.TurnOff();
    for(uint8_t i=0;i<100;i++){
        this->Read();
    }
}

void ProbeController::StartProbeTest(){
    this->savedCurrent=this->currentSelector.GetSetCurrent();
    this->currentSelector.SetCurrent(this->testCurrent);
    this->currentSelector.TurnOn();
    this->probeTestTimer.start();
    ComHandler::SendSystemMessage(SystemMessage::PROBE_TEST_START,MessageType::NOTIFY,(uint8_t)this->testCurrent);
}

void ProbeController::CycleCurrent(){
    auto newCurrent=currentSelector.CycleCurrent();
    ComHandler::SendSystemMessage(SystemMessage::CURRENT_CHANGED,
                MessageType::NOTIFY,
                (uint8_t)newCurrent);
}

CurrentValue ProbeController::GetSetCurrent(){
    return this->currentSelector.GetSetCurrent();
}

void ProbeController::SetCurrent(CurrentValue current){
    this->currentSelector.SetCurrent(current);
}

void ProbeController::TurnOffSrc(){
    this->currentSelector.TurnOff();
}

void ProbeController::TurnOnSrc(){
    this->currentSelector.TurnOn();
}

void ProbeController::Read(){
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        results[i]=probes[i]->Read();
        results[i].check(this->currentPercent);
    }
}

void ProbeController::GetProbeResults(ProbeResult* fill){
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        fill[i]=this->results[i];
    }
}