#pragma once
#include <ArduinoComponents.h>
#include "heater_constants.h"
#include "TemperatureSensor.hpp"
#include "HeaterConfiguration.hpp"
#include "../StateMachine/StateMachine.hpp"
#include "./PID/PID.hpp"
#include "./PID/PID_AutoTune.hpp"
#include "../StationTimer.hpp"

using namespace components;

class Heater:public Component{
	typedef void(Heater::*RunFunc)(void);
public:
	Heater(const HeaterConfig& config,int tempSp,unsigned long windowSize);
	Heater();
	void SetConfiguration(const HeaterConfig& config,unsigned long windowSize);
	void Initialize();
	void UpdatePid(HeaterTuneResult newPid);
	void TurnOn();
	void TurnOff();
	
	bool IsTuning();
	bool IsComplete();
	bool TempOkay();

	void SetWindowSize(unsigned long windowSize);
	
	HeaterResult Read();
	HeaterResult GetHeaterResult();
	void ChangeSetpoint(int setPoint);

	void StartTuning();
	void StopTuning();
	void RunAutoTune();
	void RunPid();

	void MapTurningComplete(TuningCompleteCallback cbk){
		this->tuningCompleteCb=cbk;	
	}

private:
	void OutputAction(unsigned long now);
	void PrintTuning(bool completed);	


private:
	TemperatureSensor 	ntc;
	//DigitalOutput 		output;
	uint8_t 			relayPin;
	
	PID  				pid;
	PID_AutoTune		autoTuner;
	HeaterResult		result;
	float tempDeviation;
	float tempSetPoint;
	float kp, ki, kd;
	unsigned long windowLastTime,WindowSize;
	float pidOutput=0;
	float temperature=0;
	bool tempOk = false;
	bool isTuning=false;
	bool isComplete=false;
	bool relayState=false;
	int id=-1;
	HeaterMode mode,nextMode;
	HeatState heaterState=HeatState::Off;
	StationTimer timer;
	TuningCompleteCallback tuningCompleteCb=[](HeaterTuneResult){};
	virtual void privateLoop()override;
};



