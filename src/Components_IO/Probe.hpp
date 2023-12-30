#pragma once
#include <ArduinoComponents.h>
#include "CurrentSensor.hpp"
#include "VoltageSensor.hpp"
#include "../Configuration/Configuration.hpp"
#include "../util.hpp"

using namespace components;

typedef struct ProbeResult{
	double voltage;
	double current;
}ProbeResult;

class Probe:public Component{
public:
	Probe(const ProbeConfig& config)
		:Component(),voltSensor(config.voltageConfig),
		currentSensor(config.currentConfig),
		readInterval(config.readInterval){

		readTimer.onInterval([&](){
			this->Read();
		},this->readInterval);
		RegisterChild(this->readTimer);
	}

	ProbeResult Read(){
		this->probeResult.voltage=this->voltSensor.ReadVoltage();
		this->probeResult.current=this->currentSensor.ReadCurrent();
	}

	ProbeResult GetProbeReading(){
		return this->probeResult;
	}

private:
	Timer readTimer;
	ProbeResult		probeResult;
	unsigned long readInterval;
	ProbeConfig configuration;
	VoltageSensor voltSensor;
	CurrentSensor currentSensor;
	void privateLoop() {
		//this->ReadVoltage();
	}
};