#pragma once
#include <ArduinoComponents.h>
#include "CurrentSensor.hpp"
#include "VoltageSensor.hpp"
#include "ProbeConfiguration.hpp"
#include "probe_constants.h"

using namespace components;

typedef struct ProbeResult{
	float voltage=0;
	float current=0;
	bool okay=false;

	void check(float currentPercent,int setCurrent){
		okay=(current>=(setCurrent*(currentPercent/100.0f)));
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
	Probe();
	void Setup(const ProbeConfig& config);
	ProbeResult Read(bool filtered=true);
	ProbeResult GetProbeReading();

private:
	ProbeResult		probeResult;
	VoltageSensor 	voltSensor;
	CurrentSensor 	currentSensor;
	Timer			readTimer;
	void privateLoop() { }
};