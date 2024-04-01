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
    ProbeController();
    void Setup(const ProbeControllerConfig& config);
    void Initialize();
    void Read();
    void TurnOnSrc();
    void TurnOffSrc();
    void CycleCurrent();
    CurrentValue GetSetCurrent();
    void SetCurrent(CurrentValue current);
    void GetProbeResults(ProbeResult* results);

private:
    Probe           probes[PROBE_COUNT];
    ProbeResult     results[PROBE_COUNT];
    CurrentSelector currentSelector;
    CurrentValue    testCurrent;
    double          currentPercent;
    Timer           readTimer;
    void privateLoop(){ }


};