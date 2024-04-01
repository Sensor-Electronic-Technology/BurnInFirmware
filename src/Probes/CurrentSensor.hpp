#pragma once
#include <ArduinoComponents.h>
#include "ProbeConfiguration.hpp"
#include "probe_constants.h"

using namespace components;

class CurrentSensor:public Component{
public:
	CurrentSensor(PinNumber pin);
	CurrentSensor(const CurrentSensorConfig& config);
	CurrentSensor();
	void Setup(const CurrentSensorConfig& config);
	double ReadCurrent();
	float GetCurrent();

private:
	AnalogInput currentIn;
	double current=0;
	double fWeight=1;

	void privateLoop() {
		//this->ReadCurrent();
	}
};