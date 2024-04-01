#pragma once
#include <ArduinoComponents.h>
#include "heater_constants.h"
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



class Heater:public Component{
	typedef void(Heater::*RunFunc)(void);
public:
	Heater(const HeaterConfig& config);
	Heater();
	void SetConfiguration(const HeaterConfig& config);
	void Initialize();
	void UpdatePid(HeaterTuneResult newPid);
	void SwitchMode(HeaterMode nextMode);
	void TurnOn();
	void TurnOff();
	void StartTuning();
	void StopTuning();
	void RunAutoTune();
	void PrintTuning(bool completed);
	bool IsTuning();
	void RunPid();
	bool TempOkay();
	void OutputAction(unsigned long now);
	HeaterResult Read();
	HeaterResult GetHeaterResult();
	void ChangeSetpoint(int setPoint);
	void MapTurningComplete(TuningCompleteCallback cbk){
		this->tuningCompleteCb=cbk;	
	}

private:
	TemperatureSensor 	ntc;
	DigitalOutput 		output;
	HeaterState 		heaterState=HeaterState::Off;
	PID  				pid;
	PID_AutoTune		autoTuner;
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
	HeaterMode heaterMode;
	RunFunc	run[2];
	TuningCompleteCallback tuningCompleteCb=[](HeaterTuneResult){};
	virtual void privateLoop()override;
};



