#pragma once
#include <ArduinoComponents.h>
#include "../constants.h"
#include "../Components_IO/TemperatureSensor.hpp"
#include "../Configuration/Configuration.hpp"
#include "../ExternalLibraries/PID_AutoTune_v0.h"
#include <PID_v1.h>

using namespace components;


enum HeaterState {
	On,
	Off
};

class Heater:public Component{
public:
/*	Heater(PinNumber sensorPin,PinNumber heatPin,float a,float b,float c) 
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
*/
	Heater(const HeaterConfig& config) 
			:Component(),ntc(config.ntcConfig),output(config.Pin),
			kp(config.kp),kd(config.kd),ki(config.ki),WindowSize(config.windowSize),
			configuration(config){
		this->pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,
		this->kp,this->ki,this->kd,DIRECT);
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
		this->heaterState=HeaterState::On;
		this->windowLastTime=millis();
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
		this->temperature=this->ntc.Read();
		if(this->heaterState==HeaterState::On){
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
		return this->temperature;
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

	const HeaterConfig& GetConfig(){
		return this->configuration;
	}

private:
	TemperatureSensor 	ntc;
	DigitalOutput 		output;
	HeaterState 		heaterState; 
	HeaterConfig		configuration;
	PID  				*pid;

	double tempSetPoint =DEFAULT_TEMPSP;
	double kp=KP_DEFAULT, ki=KI_DEFAULT, kd=KD_DEFAULT;
	unsigned long windowLastTime,WindowSize=dutyTime;
	double pidOutput=0;
	double temperature=0;
	bool tempOk = false;
	
	void privateLoop() {  
		this->ProcessPid();
	}
};



