#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "Probe.hpp"
#include "ProbeConfiguration.hpp"
#include "../constants.h"

using namespace std;
using namespace components;


class ProbeController:public Component{
public:
    ProbeController(const ProbeControllerConfig& config);
    void Initialize();
    void Read();
    Array<ProbeResult,PROBE_COUNT> GetProbeResults();

private:
    Array<Probe*,PROBE_COUNT>        probes;
    Array<ProbeResult,PROBE_COUNT>   results;
    Timer                            readTimer;
    void privateLoop(){ }


};