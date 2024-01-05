#pragma once
#include <ArduinoComponents.h>
#include "../Configuration/ProbeConfiguration.hpp"
#include "../constants.h"

using namespace components;

class CurrentSensor:public Component{
public:

	CurrentSensor(PinNumber pin):Component(),
		currentIn(pin),
		fWeight(DEFAULT_FWEIGHT){
		this->current=0;
	}

	CurrentSensor(const CurrentSensorConfig& config)
			:Component(),currentIn(config.Pin),
			fWeight(config.fWeight){
	}

	double ReadCurrent() {
		int value=this->currentIn.read();
		value=map(value,ADC_MIN,ADC_MAX,CURRENT_MIN,CURRENT_MAX);
		this->current+=(((float)value)-this->current)*this->fWeight;
		return this->current;
	}

	float GetCurrent(){
		return this->current;
	}

private:
	AnalogInput currentIn;
	double current=0;
	double fWeight=1;

	void privateLoop() {
		//this->ReadCurrent();
	}
};