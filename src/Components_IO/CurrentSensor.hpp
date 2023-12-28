#pragma once
#include <ArduinoComponents.h>
#include "../util.hpp"

using namespace components;

#define CurrentFilter   0.02f

class CurrentSensor:public Component{
public:

	CurrentSensor(PinNumber pin):Component(),currentIn(pin){
		this->current=0;
	}

	void ReadCurrent() {
		int value=this->currentIn.read();
		value=map(value,MinADC,MaxADC,MinCurrent,MaxCurrent);
		this->current+=(((float)value)-this->current)*CurrentFilter;
	}

	float GetCurrent() {
		return this->current;
	}

private:
	AnalogInput currentIn;

	Timer readTimer;
	float current=0;

	void privateLoop() {
		this->ReadCurrent();
	}
};