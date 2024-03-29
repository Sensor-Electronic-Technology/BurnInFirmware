#pragma once
#include <ArduinoComponents.h>
#include "HeaterConfiguration.hpp"
#include "heater_constants.h"

using namespace components;

class TemperatureSensor:public Component{
public:
	TemperatureSensor(PinNumber pin,float a,float b,float c);
	TemperatureSensor(const NtcConfig& config);
	double Read();
	float GetTemperature();
	int Pin(){return this->input.pinNumber();}
	void UpdateConfig(const NtcConfig& config);

private:
	AnalogInput input;
	NtcConfig	configuration;
	double		fWeight;		
	double      temperature;
	double      aValue;
	double      bValue;
	double      cValue;
	
	void privateLoop() {
		//this->Read();
	}
};