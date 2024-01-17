#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "Probe.hpp"
#include "ProbeConfiguration.hpp"
#include "CurrentSelector.hpp"
#include "probe_constants.h"

using namespace components;


class ProbeController:public Component{
public:
    ProbeController(const ProbeControllerConfig& config);
    void Initialize();
    void Read();
    void TurnOnSrc();
    void TurnOffSrc();
    void CycleCurrent();
    CurrentValue GetSetCurrent();
    void SetCurrent(CurrentValue current);
    Array<ProbeResult,PROBE_COUNT> GetProbeResults();

private:
    Array<Probe*,PROBE_COUNT>        probes;
    Array<ProbeResult,PROBE_COUNT>   results;
    CurrentSelector                  currentSelector;
    CurrentValue                     testCurrent;
    double                           currentPercent;
    Timer                            readTimer;
    void privateLoop(){ }


};