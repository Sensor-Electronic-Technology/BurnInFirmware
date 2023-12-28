#pragma once
#include <ArduinoComponents.h>
#include "../util.hpp"

using namespace components;

class TemperatureSensor:public Component{
public:
	TemperatureSensor(PinNumber pin,float a,float b,float c) :Component(),input(pin),
		temperature(0),aValue(a),bValue(b),cValue(c) {    }

	float Read(){
		double aValue = this->input.read();
		double ntc_res=Rref/(MaxADC/aValue-1);
		//Steinhart and Hart Equation 1/A+Bln(R)+C[ln(R)]^3  
		double temp=(1/(this->aValue+(this->bValue*log(ntc_res))+(this->cValue*pow(log(ntc_res),3))))-273.15;
		this->temperature+=(temp-this->temperature)*tempfilter;
		return this->temperature;
	}

	float GetTemperature() {
		return this->temperature;
	}

private:
	AnalogInput input;
	Timer       readTimer;
	float       temperature=0;
	float       aValue;
	float       bValue;
	float       cValue;


	void privateLoop() {
		this->Read();
	}
};