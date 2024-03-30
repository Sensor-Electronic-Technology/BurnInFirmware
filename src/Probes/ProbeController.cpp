#include "ProbeController.hpp"

ProbeController::ProbeController(const ProbeControllerConfig& config)
    :Component(),
    currentSelector(config.currentSelectConfig),
    testCurrent(config.probeTestCurrent),
    currentPercent(config.probeCurrentPercent)
{
    for(int i=0;i<PROBE_COUNT;i++){
        probes.push_back(new Probe(config.probeConfigs[i]));
        ProbeResult result; 
        results.push_back(result);
    }
    
    this->readTimer.onInterval([&]{
        this->Read();
    },config.readInterval);
    RegisterChild(this->readTimer);
}

void ProbeController::Initialize(){
    this->currentSelector.TurnOff();
    for(int i=0;i<100;i++){
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
    for(int i=0;i<PROBE_COUNT;i++){
        results[i]=probes[i]->Read();
        results[i].check(this->currentPercent);
    }
}

Array<ProbeResult,PROBE_COUNT> ProbeController::GetProbeResults(){
    return this->results;
}