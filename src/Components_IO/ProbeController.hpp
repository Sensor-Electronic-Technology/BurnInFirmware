#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "Probe.hpp"
#include "../Configuration/Configuration.hpp"
#include "../constants.h"

using namespace std;
using namespace components;


class ProbeController:public Component{
public:

    ProbeController(const ProbeControllerConfig& config):Component(){
        for(int i=0;i<PROBE_COUNT;i++){
            probes.push_back(new Probe(config.probeConfigs[i]));
        }
    }

    void Initialize(){
        for(int i=0;i<100;i++){
            this->Read();
        }
    }

     Read(){
        for(int i=0;i<PROBE_COUNT;i++){
            results[i]=probes[i]->Read();
        }
    }

private:
    Array<Probe*,PROBE_COUNT>        probes;
    Array<ProbeResult,PROBE_COUNT>   results;
    Timer                            readTimer;
    void privateLoop(){ }


};