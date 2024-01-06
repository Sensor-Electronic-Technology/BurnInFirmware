#include "ProbeController.hpp"

ProbeController::ProbeController(const ProbeControllerConfig& config):Component(){
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
    for(int i=0;i<100;i++){
        this->Read();
    }
}

void ProbeController::Read(){
    for(int i=0;i<PROBE_COUNT;i++){
        results[i]=probes[i]->Read();
    }
}

Array<ProbeResult,PROBE_COUNT> ProbeController::GetProbeResults(){
    return this->results;
}