#pragma once
#include <ArduinoComponents.h>
#include "../util.hpp"
#include "../Components_IO/TemperatureSensor.hpp"
#include "../ExternalLibraries/PID_AutoTune_v0.h"
#include <PID_v1.h>

using namespace components;
#define PIDVERSION 		2
#define KP_DEFAULT		2
#define KI_DEFAULT		5
#define KD_DEFAULT		1
#define Window_Default	500
#define DefaultTempSP	85

enum HeaterState {
	On,
	Off
};

#if PIDVERSION == 1
class heater:public Component{
public:
	heater(PinNumber sensorPin,PinNumber heatPin,float a,float b,float c) 
		:Component(),
		output(heatPin), 
		ntc(sensorPin,a,b,c) {
		
		this->lastErr = this->err;
		//this->err = this->tempSP - this->temperature;
		
		RegisterChild(this->dutyTimer);
		RegisterChild(this->runTimer);
		RegisterChild(this->ntc);

		this->dutyTimer.onInterval([&](){
			SetHeaterDuty();
		}, dutyTime);

		this->runTimer.onInterval([&]() {
			SetOuput();
		}, runtime);
	}

	void SetHeaterDuty(){
		temperature = this->ntc.GetTemperature();
		lastErr = err; 
		err = tempSP - temperature;
		if (abs(err) * 100 / this->tempSetPoint < tempDeviation) {
			this->tempOk = true;
		} else {
			this->tempOk = false;
		}

		iTerm = iTerm + err * Ki; 
		if (iTerm > iTermMax) {
			iTerm = iTermMax;
		} else if (iTerm < ITermMin) {
			iTerm = ITermMin;
		}

		pTerm = Kp * err;
		dTerm = dTerm + ((Kd * (err - lastErr)) - dTerm) * dTermFact;
		Duty = pTerm + iTerm + dTerm;

		if (Duty > maxDuty) {
			Duty= maxDuty;
		} else if (Duty < minDuty) {
			Duty = minDuty;
		}

		if (temperature > tempHLimit) {
			heaterDuty = 0;
		} else if (temperature < tempLLimit) {
			heaterDuty = maxDuty;
		} else if (temperature < tempHLimit) {
			heaterDuty = (int(Duty) * maxDuty) / maxDuty;
			if ((heaterDuty > hiDuty) || (heaterDuty > maxDuty)) {
				heaterDuty = maxDuty;
			} else if (heaterDuty < lowDuty) {
				heaterDuty = minDuty;
			}
		}
	}

	void SetOuput() {
		if (millisTime() >= (lastOutputTime + outputPeriod)) {
			lastOutputTime = lastOutputTime + outputPeriod;
			if (heaterDuty > minDuty) {
				this->output.high();
			} else {
				this->output.low();
			}
		}
		if (millisTime() >= (lastOutputTime + (outputPeriod / maxDuty) * heaterDuty)) {
			this->output.low();
		}
	}

	float GetTemperature() {
		return this->ntc.GetTemperature();
	}

	void ReadTempManual() {
		this->ntc.Read();
	}
	
	bool TempOK() {
		return this->tempOk;
	}

	void ChangeSetpoint(int setPoint) {
		this->tempSetPoint = setPoint;
	}

	void SetState(HeaterState state) {
		if (state==HeaterState::On) {
			this->tempSP = this->tempSetPoint;
		} else {
			this->tempSP = 0;
		}
	}

	bool ToggleHeating() {
		if (this->tempSP != this->tempSetPoint) {
			this->tempSP = this->tempSetPoint;
			return true;
		} else {
			this->tempSP = 0;
			return false;
		}
	}

private:
	ntc ntc;
	DigitalOutput output;
	Timer dutyTimer;	
	Timer runTimer;

	volatile int heaterDuty = maxDuty;
	volatile float Duty = maxDuty;
	volatile float pTerm = 0.0;
	volatile float err = 0.0;
	volatile float iTerm = 0.0;
	volatile float dTerm = 0.0;
	volatile float lastErr = 0.0;

	volatile float tempSP=0;
	volatile float temperature=0;
	volatile float tempSetPoint = DefaultSetPoint;
	unsigned long lastOutputTime = 0;
	bool tempOk = false;

	void privateLoop() {  }
};
#else

typedef struct HeaterConfig{
	double kp,kd,ki;
	unsigned long windowSize;
	HeaterConfig(){
		this->kp=2;
		this->ki=5;
		this->kd=1;
		this->windowSize=500;
	}
	HeaterConfig(const HeaterConfig& config){
		this->kd=config.kd;
		this->ki=config.ki;
		this->kp=config.kp;
		this->windowSize=config.windowSize;
	}
	HeaterConfig& operator=(const HeaterConfig& rhs){
		this->kp=rhs.kp;
		this->ki=rhs.ki;
		this->kd=rhs.kd;
		this->windowSize=rhs.windowSize;
		return *this;
	}
}HeaterConfig;

class Heater:public Component{
public:
	Heater(PinNumber sensorPin,PinNumber heatPin,float a,float b,float c) 
		:Component(),
		output(heatPin), 
		ntc(sensorPin,a,b,c) {
		
		//this->lastErr = this->err;
		//this->err = this->tempSP - this->temperature;
		pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,
				this->kp,this->ki,this->kd,DIRECT);
		
		// RegisterChild(this->dutyTimer);
		// RegisterChild(this->runTimer);
		RegisterChild(this->ntc);
	}

	Heater(PinNumber sensorPin,PinNumber heatPin,const HeaterConfig& config,
	float a,float b,float c) :Component(),output(heatPin), ntc(sensorPin,a,b,c){
		
		this->pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,
				this->kp,this->ki,this->kd,DIRECT);
		this->kp=config.kp;
		this->kd=config.kd;
		this->ki=config.ki;
		this->WindowSize=config.windowSize;
		this->configuration=config;
		RegisterChild(this->ntc);
	}

	void UpdateConfiguratiuon(const HeaterConfig& config){
		this->kp=config.kp;
		this->kd=config.kd;
		this->ki=config.ki;
		this->WindowSize=config.windowSize;
		this->configuration=config;
		this->pid->SetTunings(this->kp,this->ki,this->kd);
		this->pid->SetOutputLimits(0,this->WindowSize);
	}

	void TurnOn(){
		this->heaterState=HeaterState::Off;
		this->windowLastTime=millisTime();
		this->pid->SetOutputLimits(0,this->WindowSize);
		this->pid->SetMode(AUTOMATIC);
	}

	void TurnOff(){
		this->heaterState=HeaterState::Off;
	}

	void ManualControl(bool on){
		if(on){
			this->output.high();
		}else{
			this->output.low();
		}
	}

	void ProcessPid(){
		if(this->heaterState==HeaterState::On){
			this->temperature=this->ntc.Read();
			this->pid->Compute();
			if(millisTime()-this->windowLastTime>this->WindowSize){
				this->windowLastTime+=this->WindowSize;
			}
			if(this->pidOutput<millisTime()-this->windowLastTime){
				this->output.high();
			}else{
				this->output.low();
			}
		}else{
			this->output.low();
		}
	}

	float GetTemperature() {
		return this->ntc.GetTemperature();
	}

	double ReadTempManual() {
		return this->ntc.Read();
	}
	
	bool TempOK() {
		return this->tempOk;
	}

	void ChangeSetpoint(int setPoint) {
		this->tempSetPoint = setPoint;
	}

	bool ToggleHeating() {
		if(this->heaterState==HeaterState::On){
			this->TurnOff();
		}else{
			this->TurnOn();
		}
	}

	PID* GetPid(){
		return this->pid;
	}

	HeaterConfig GetConfig(){
		return this->configuration;
	}

private:
	TemperatureSensor 			ntc;
	DigitalOutput 	output;
	Timer 			dutyTimer;	
	Timer 			runTimer;
	PID  			*pid;
	HeaterState 	heaterState; 
	HeaterConfig	configuration;
	PID_ATune		*aTune;

	
	double tempSetPoint =DefaultTempSP;
	double kp=KP_DEFAULT, ki=KI_DEFAULT, kd=KD_DEFAULT;
	unsigned long windowLastTime,WindowSize=dutyTime;
	double pidOutput=0;
	double temperature=0;
	bool tempOk = false;
	byte ATuneModeRemember=2;

	void privateLoop() {  
		this->ProcessPid();
	}
};
#endif



