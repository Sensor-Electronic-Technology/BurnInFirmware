#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "Probe.hpp"
#include "../Configuration/ProbeConfiguration.hpp"
#include "../constants.h"

using namespace std;
using namespace components;


class ProbeController:public Component{
public:

    ProbeController(const ProbeControllerConfig& config):Component(){
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

    void Initialize(){
        for(int i=0;i<100;i++){
            this->Read();
        }
    }

    void Read(){
        for(int i=0;i<PROBE_COUNT;i++){
            results[i]=probes[i]->Read();
        }
    }

    Array<ProbeResult,PROBE_COUNT> GetProbeResults(){
        return this->results;
    }

private:
    Array<Probe*,PROBE_COUNT>        probes;
    Array<ProbeResult,PROBE_COUNT>   results;
    Timer                            readTimer;
    void privateLoop(){ }


};