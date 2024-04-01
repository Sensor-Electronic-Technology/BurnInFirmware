#include "Probe.hpp"

Probe::Probe(const ProbeConfig& config)
    :Component(),voltSensor(config.voltageConfig),
    currentSensor(config.currentConfig){
}

Probe::Probe():Component(){
}

void Probe::Setup(const ProbeConfig& config){
    this->voltSensor.Setup(config.voltageConfig);
    this->currentSensor.Setup(config.currentConfig);
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