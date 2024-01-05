#pragma once
#include <ArduinoComponents.h>
#include "../constants.h"
#include "../Components_IO/TemperatureSensor.hpp"
#include "../Configuration/HeaterConfiguration.hpp"
#include "../Tools/PID.hpp"
#include "../Tools/PID_AutoTune.hpp"

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
	Heater(const HeaterConfig& config) 
		:Component(),
		id(config.HeaterId),
		ntc(config.ntcConfig),
		output(config.Pin),
		kp(config.pidConfig.kp),
		kd(config.pidConfig.kd),
		ki(config.pidConfig.ki),
		tempDeviation(config.tempDeviation),
		WindowSize(config.pidConfig.windowSize),
		heaterState(HeaterState::Off){

		this->tempSetPoint=DEFAULT_TEMPSP;
		this->pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,
		this->kp,this->ki,this->kd);
		this->pid->SetOutputRange(0,this->WindowSize,true);
		this->autoTuner=new PID_AutoTune(&this->temperature,&this->pidOutput,this->tempSetPoint,
			this->pid->GetSampleTime(),15);
		this->autoTuner->SetOutputRange(0,this->WindowSize);
		Serial.print("Sample Time=");Serial.print(this->pid->GetSampleTime());
	}

	void Initialize(){
		this->output.low();
	}

	void UpdateConfiguratiuon(const HeaterConfig& config){
		this->kp=config.pidConfig.kp;
		this->kd=config.pidConfig.kd;
		this->ki=config.pidConfig.ki;
		this->WindowSize=config.pidConfig.windowSize;
	}

	void TurnOn(){
		this->heaterState=HeaterState::On;
		this->windowLastTime=millis();
		this->pid->Start();
	}

	void TurnOff(){
		this->heaterState=HeaterState::Off;
	}

	void StartTuning(){
		this->autoTuner->StartTuning();
		this->isTuning=true;
	}

	void StopTuning(){
		this->isTuning=false;
		this->PrintTuning(false);
	}

	void TunePidV2(){
		auto now=millis();
		this->Read();
		if(this->isTuning){
			if(this->autoTuner->Tune()){
				if(!this->relayState){
					this->relayState=true;
					this->output.high();
					Serial.print("H");Serial.print("[");Serial.print(this->id);Serial.print("]: ");
					Serial.print("Output high");Serial.print(" Pid Out: ");Serial.println(pidOutput);
				}
			}else{
				if(this->relayState){
					this->relayState=false;
					this->output.low();
					Serial.print("H");Serial.print("[");Serial.print(this->id);Serial.print("]: ");
					Serial.print("Output Low");Serial.print(" Pid Out: ");Serial.println(pidOutput);
				}
			}
			this->OutputAction(now);
			if(this->autoTuner->Finished()){
				this->isTuning=false;
				this->PrintTuning(true);
			}
		}
	}

	void PrintTuning(bool completed){
		Serial.print("H");Serial.print("[");Serial.print(this->id);Serial.print("]: ");
		Serial.print("kp=");Serial.print(this->autoTuner->GetKp());
		Serial.print(" , ki=");Serial.print(this->autoTuner->GetKd());
		Serial.print(" , kd=");Serial.print(this->autoTuner->GetKi());
	}

	bool IsTuning(){
		return this->isTuning;
	}

	void ProcessPid(){
		this->Read();
		if(this->heaterState==HeaterState::On){
			auto now=millis();
			if(this->pid->Run()){
				this->windowLastTime=now;
			}
			this->OutputAction(now);
		}else{
			this->output.low();
		}
	}
	

	void OutputAction(unsigned long now){
		if(!this->relayState && this->pidOutput>(now-this->windowLastTime)){
			this->relayState=true;
			this->output.high();
			#if HEATER_DEBUG==1
			Serial.print("Output high Now: ");Serial.print(now);Serial.print(" Last: ");Serial.print(windowLastTime);
			Serial.print(" Pid Out: ");Serial.println(pidOutput);
			#endif
		}else if(this->relayState && this->pidOutput<(now-this->windowLastTime)){
			this->relayState=false;
			this->output.low();
			#if HEATER_DEBUG==1
			Serial.print("Output low Now: ");Serial.print(now);Serial.print(" Last: ");Serial.print(windowLastTime);
			Serial.print(" Pid Out: ");Serial.println(pidOutput);
			#endif
		}
	}

	HeaterResult Read(){
		this->temperature=this->ntc.Read();
		auto max=this->tempSetPoint+(this->tempSetPoint*this->tempDeviation);
		auto min=this->tempSetPoint-(this->tempSetPoint*this->tempDeviation);
		HeaterResult data;
		this->result.temperature=temperature;
		this->result.tempOkay=(this->temperature<=max && this->temperature>=min);
		this->result.state=this->relayState;	
		data=result;
		return data;
	}

	HeaterResult GetHeaterResult(){
		HeaterResult data=this->result;
		return data;
	}

	void ChangeSetpoint(int setPoint) {
		this->tempSetPoint = setPoint;
	}

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



