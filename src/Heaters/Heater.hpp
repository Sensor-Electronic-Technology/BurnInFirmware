#pragma once
#include <ArduinoComponents.h>
#include "../constants.h"
#include "TemperatureSensor.hpp"
#include "HeaterConfiguration.hpp"
#include "./PID/PID.hpp"
#include "./PID/PID_AutoTune.hpp"

using namespace components;

#define HEATER_DEBUG 	1

enum HeaterState {
	On,
	Off
};

typedef struct HeaterResult{
	double temperature=0;
	bool state=false;
	bool tempOkay=false;

	HeaterResult operator=(const HeaterResult& rhs){
		this->temperature=rhs.temperature;
		this->state=rhs.state;
		this->tempOkay=rhs.tempOkay;
		return *this;
	}

}HeaterResult;

typedef struct TuneParameters{
	double kp=0,ki=0,kd=0;
	unsigned long output=0;
}TuneParameters;

class Heater:public Component{
public:
	Heater(const HeaterConfig& config);
	void Initialize();
	void UpdateConfiguratiuon(const HeaterConfig& config);
	void TurnOn();
	void TurnOff();
	void StartTuning();
	void StopTuning();
	void TunePidV2();
	void PrintTuning(bool completed);
	bool IsTuning();
	void ProcessPid();
	void OutputAction(unsigned long now);
	HeaterResult Read();
	HeaterResult GetHeaterResult();
	void ChangeSetpoint(int setPoint);

private:
	TemperatureSensor 	ntc;
	DigitalOutput 		output;
	HeaterState 		heaterState=HeaterState::Off;
	PID  				*pid;
	PID_AutoTune		*autoTuner;

	HeaterResult		result;
	double tempDeviation;
	double tempSetPoint;
	double kp, ki, kd;
	unsigned long windowLastTime,WindowSize;
	double pidOutput=0;
	double temperature=0;
	bool tempOk = false;
	bool isTuning=false;
	bool relayState=false;
	int id=0;
	
	void privateLoop() {  
		this->ProcessPid(); 
	}
};



