#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "Probe.hpp"
#include "ProbeConfiguration.hpp"
#include "CurrentSelector.hpp"
#include "probe_constants.h"
#include "../StationTimer.hpp"
#include "../Communication/ComHandler.hpp"

using namespace components;


class ProbeController:public Component{
public:
    ProbeController(const ProbeControllerConfig& config);
    ProbeController();
    void Setup(const ProbeControllerConfig& config);
    void Initialize();
    void ReInitialize();
    void Read();
    void TurnOnSrc();
    void TurnOffSrc();
    void CycleCurrent();
    void StartProbeTest();
    CurrentValue GetSetCurrent();
    void SetCurrent(CurrentValue current);
    void GetProbeResults(ProbeResult* fill);

private:
    Probe           probes[PROBE_COUNT];
    ProbeResult     results[PROBE_COUNT];
    CurrentSelector currentSelector;
    CurrentValue    savedCurrent;
    CurrentValue    testCurrent;
    float           currentPercent;
    Timer           readTimer;
    StationTimer    probeTestTimer;
    int             probeTestTime;
    unsigned long   readInterval=1000;
    void privateLoop(){ }
};