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
		this->inputPin=config.Pin;
		this->aValue=config.aCoeff;
		this->bValue=config.bCoeff;
		this->cValue=config.cCoeff;
		this->fWeight=config.fWeight;
	}
	float Read();
	float GetTemperature();
	int Pin(){return this->inputPin;}
	void UpdateConfig(const NtcConfig& config);

private:
	uint8_t 	inputPin;
	float		fWeight;		
	float      temperature;
	float      aValue;
	float      bValue;
	float      cValue;
	
	void privateLoop() {
		//this->Read();
	}
};