#pragma once
#include <ArduinoComponents.h>
#include "CurrentSensor.hpp"
#include "VoltageSensor.hpp"
#include "ProbeConfiguration.hpp"

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
	Probe(const ProbeConfig& config);
	ProbeResult Read();
	ProbeResult GetProbeReading();

private:
	ProbeResult		probeResult;
	ProbeConfig 	configuration;
	VoltageSensor 	voltSensor;
	CurrentSensor 	currentSensor;
	Timer			readTimer;
	void privateLoop() { }
};