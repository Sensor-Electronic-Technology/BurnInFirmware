#pragma once
#include <ArduinoComponents.h>
#include "../Configuration/Configuration.hpp"
#include "../constants.h"

using namespace components;

class VoltageSensor:public Component{
public:
	VoltageSensor(PinNumber voltagePin)
        :Component(),voltageIn(voltagePin),
        fWeight(DEFAULT_FWEIGHT) {
		this->voltage=0.0;
	}

	VoltageSensor(const VoltageSensorConfig& config)
		:Component(),
        voltageIn(config.Pin),
        fWeight(config.fWeight),
        configuration(config){
    } 

	double ReadVoltage() {
		float value=this->voltageIn.read();
		value=map(value,ADC_MIN,ADC_MAX,VOLTAGE_MIN,VOLTAGE_MIN);
		this->voltage += (((float)value) - this->voltage) * this->fWeight;
        return this->voltage;
	}

	float GetVoltage() {
		return this->voltage;
	}

private:
	AnalogInput voltageIn;
    VoltageSensorConfig configuration;
	double voltage;
    double fWeight;

	void privateLoop() {
		//this->ReadVoltage();
	}
};