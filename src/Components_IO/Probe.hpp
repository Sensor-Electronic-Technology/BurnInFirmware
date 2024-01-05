#pragma once
#include <ArduinoComponents.h>
#include "CurrentSensor.hpp"
#include "VoltageSensor.hpp"
#include "../Configuration/ProbeConfiguration.hpp"

using namespace components;

typedef struct ProbeResult{
	double voltage=0;
	double current=0;
	ProbeResult operator=(const ProbeResult& rhs){
		this->voltage=rhs.voltage;
		this->current=rhs.current;
		return *this;
	}
}ProbeResult;

class Probe:public Component{
public:
	Probe(const ProbeConfig& config)
		:Component(),voltSensor(config.voltageConfig),
		currentSensor(config.currentConfig){
	}

	ProbeResult Read(){
		this->probeResult.voltage=this->voltSensor.ReadVoltage();
		this->probeResult.current=this->currentSensor.ReadCurrent();
		ProbeResult data=this->probeResult;
		return data;
	}

	ProbeResult GetProbeReading(){
		ProbeResult data=this->probeResult;
		return data;
	}

private:
	ProbeResult		probeResult;
	ProbeConfig 	configuration;
	VoltageSensor 	voltSensor;
	CurrentSensor 	currentSensor;
	Timer			readTimer;
	void privateLoop() { }
};