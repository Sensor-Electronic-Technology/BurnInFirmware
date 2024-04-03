#include "ProbeController.hpp"

ProbeController::ProbeController(const ProbeControllerConfig& config)
    :Component(),
    currentSelector(config.currentSelectConfig),
    testCurrent(config.probeTestCurrent),
    currentPercent(config.probeCurrentPercent)
{
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probes[i]=Probe(config.probeConfigs[i]);
        RegisterChild(this->probes[i]);
        this->results[i]=ProbeResult();
    }
    
    this->readTimer.onInterval([&]{
        this->Read();
    },config.readInterval);
    RegisterChild(this->readTimer);
}

ProbeController::ProbeController():Component(){

}

void ProbeController::Setup(const ProbeControllerConfig& config){
    this->currentSelector.Setup(config.currentSelectConfig);
    this->testCurrent=config.probeTestCurrent;
    this->currentPercent=config.probeCurrentPercent;
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        this->probes[i]=Probe(config.probeConfigs[i]);
        RegisterChild(this->probes[i]);
        this->results[i]=ProbeResult();
    }
    
    this->readTimer.onInterval([&]{
        this->Read();
    },config.readInterval);
    RegisterChild(this->readTimer);
}

void ProbeController::Initialize(){
    this->currentSelector.TurnOff();
    for(uint8_t i=0;i<100;i++){
        this->Read();
    }
}

void ProbeController::CycleCurrent(){
    this->currentSelector.CycleCurrent();
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
        results[i]=probes[i].Read();
        results[i].check(this->currentPercent);
    }
}

void ProbeController::GetProbeResults(ProbeResult* fill){
    for(uint8_t i=0;i<PROBE_COUNT;i++){
        fill[i]=this->results[i];
    }
}