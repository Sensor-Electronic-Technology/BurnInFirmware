#pragma once
#include <ArduinoComponents.h>
#include "CurrentSensor.hpp"
#include "VoltageSensor.hpp"
#include "ProbeConfiguration.hpp"
#include "probe_constants.h"

using namespace components;

typedef struct ProbeResult{
	double voltage=0;
	double current=0;
	bool okay=false;

	void check(double currentPercent){
		okay=(current>=(current*currentPercent));
	}

	ProbeResult operator=(const ProbeResult& rhs){
		this->voltage=rhs.voltage;
		this->current=rhs.current;
		this->okay=rhs.okay;
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
	VoltageSensor 	voltSensor;
	CurrentSensor 	currentSensor;
	Timer			readTimer;
	void privateLoop() { }
};