#pragma once
#include <ArduinoComponents.h>
#include <Arduino.h>
#include "ProbeConfiguration.hpp"
#include "probe_constants.h"

using namespace components;

class CurrentSensor:public Component{
public:
	CurrentSensor(PinNumber pin);
	CurrentSensor(const CurrentSensorConfig& config);
	CurrentSensor();
	void Setup(const CurrentSensorConfig& config);
	float ReadCurrent();
	float GetCurrent();

private:
	uint8_t currentIn;
	float current=0;
	float fWeight=1;

	void privateLoop() {
		//this->ReadCurrent();
	}
};