#pragma once
#include <ArduinoComponents.h>
#include "HeaterConfiguration.hpp"
#include "heater_constants.h"

using namespace components;

class TemperatureSensor:public Component{
public:
	TemperatureSensor(PinNumber pin,float a,float b,float c);
	
	TemperatureSensor(const NtcConfig& config);
	TemperatureSensor():Component(){

	}

	void Setup(const NtcConfig& config){
		this->configuration=config;
		this->input.setAnalogPin(config.Pin);
		this->aValue=config.aCoeff;
		this->bValue=config.bCoeff;
		this->cValue=config.cCoeff;
		this->fWeight=config.fWeight;
	}
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