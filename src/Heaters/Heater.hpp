#pragma once
#include <ArduinoComponents.h>
#include "heater_constants.h"
#include "TemperatureSensor.hpp"
#include "HeaterConfiguration.hpp"
#include "../StateMachine/StateMachine.hpp"
#include "./PID/PID.hpp"
#include "./PID/PID_AutoTune.hpp"

using namespace components;



enum HeatState {
	On,
	Off
};

typedef struct HeaterResult{
	float temperature=0;
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
	
	bool IsTuning();
	bool IsComplete();
	bool TempOkay();
	
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
	RunFunc run[2];
	Timer timer;
	TuningCompleteCallback tuningCompleteCb=[](HeaterTuneResult){};
	virtual void privateLoop()override;
};



