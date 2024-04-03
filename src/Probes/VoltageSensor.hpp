#pragma once
#include <ArduinoComponents.h>
#include "ProbeConfiguration.hpp"
#include "probe_constants.h"

using namespace components;

class VoltageSensor:public Component{
public:
	VoltageSensor(PinNumber voltagePin);
	VoltageSensor(const VoltageSensorConfig& config);
	VoltageSensor();
	void Setup(const VoltageSensorConfig& config);
	float ReadVoltage();
	float GetVoltage();
private:
	//AnalogInput voltageIn;
	uint8_t voltageIn;
	float voltage;
    float fWeight;

	void privateLoop() {
		//this->ReadVoltage();
	}
};