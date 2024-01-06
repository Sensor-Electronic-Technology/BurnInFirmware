#pragma once
#include <ArduinoComponents.h>
#include "ProbeConfiguration.hpp"
#include "../constants.h"

using namespace components;

class VoltageSensor:public Component{
public:
	VoltageSensor(PinNumber voltagePin);
	VoltageSensor(const VoltageSensorConfig& config);
	double ReadVoltage();
	float GetVoltage();
private:
	AnalogInput voltageIn;
    VoltageSensorConfig configuration;
	double voltage;
    double fWeight;

	void privateLoop() {
		//this->ReadVoltage();
	}
};