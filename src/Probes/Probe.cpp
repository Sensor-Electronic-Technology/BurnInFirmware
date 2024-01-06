#include "Probe.hpp"

Probe::Probe(const ProbeConfig& config)
    :Component(),voltSensor(config.voltageConfig),
    currentSensor(config.currentConfig){
}

ProbeResult Probe::Read(){
    this->probeResult.voltage=this->voltSensor.ReadVoltage();
    this->probeResult.current=this->currentSensor.ReadCurrent();
    ProbeResult data=this->probeResult;
    return data;
}

ProbeResult Probe::GetProbeReading(){
    ProbeResult data=this->probeResult;
    return data;
}