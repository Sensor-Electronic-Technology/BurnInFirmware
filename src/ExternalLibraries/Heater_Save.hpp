/*#pragma once
#include <ArduinoComponents.h>
#include "../constants.h"
#include "../Components_IO/TemperatureSensor.hpp"
#include "../Configuration/Configuration.hpp"
#include "../ExternalLibraries/PID_AutoTune_v0.h"
#include <PID_v1.h>
#include "../ExternalLibraries/PID_AutoTune_v0.h"

using namespace components;

enum HeaterState {
	On,
	Off
};

typedef struct HeaterResult{
	double temperature=false;
	bool state=false;
	bool tempOkay=false;
}HeaterResult;

typedef struct TuneParameters{
	double kp=0,ki=0,kd=0;
	unsigned long output=0;
}TuneParameters;

class Heater:public Component{
public:
	Heater(const HeaterConfig& config) 
		:Component(),
		ntc(config.ntcConfig),
		output(config.Pin),
		kp(config.kp),
		kd(config.kd),
		ki(config.ki),
		tempDeviation(config.tempDeviation),
		WindowSize(config.windowSize),
		heaterState(HeaterState::Off){
		this->tempSetPoint=DEFAULT_TEMPSP;
		this->pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,
		this->kp,this->ki,this->kd,DIRECT);
		this->pidAuto=new PID_ATune(&this->temperature,&this->pidOutput);
	}

	void Initialize(){
		this->output.low();
	}

	void UpdateConfiguratiuon(const HeaterConfig& config){
		this->kp=config.kp;
		this->kd=config.kd;
		this->ki=config.ki;
		this->WindowSize=config.windowSize;
		this->pid->SetTunings(this->kp,this->ki,this->kd);
		this->pid->SetOutputLimits(0,this->WindowSize);
	}

	void TurnOn(){
		this->heaterState=HeaterState::On;
		this->windowLastTime=millis();
		this->pid->SetTunings(this->kp,this->ki,this->kd);
		this->pid->SetOutputLimits(0,this->WindowSize);
		this->pid->SetMode(AUTOMATIC);
	}

	void StartAutoTune(){
		this->ChangeAutoTune();
	}

	void StopAutoTune(){
		this->ChangeAutoTune();
	}

	void Print(bool newLine){
		Serial.print("Output: ");Serial.print(this->pidOutput);
		Serial.print(" Temp: ");Serial.print(this->temperature);
		Serial.print(" SetPoint: ");Serial.print(this->tempSetPoint);
		if(!this->pidTuning){
			Serial.print(" kp: ");Serial.print(this->kp);
			Serial.print(" ki: ");Serial.print(this->ki);
			Serial.print(" kd: ");Serial.print(this->kd);
		}
		if(newLine){
			Serial.println();
		}
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
		auto result=this->Read();
		this->temperature=result.temperature;
		if(this->heaterState==HeaterState::On){
			if(this->pidTuning){
				byte val=this->pidAuto->Runtime();
				if(val!=0){
					this->pidTuning=false;
					Serial.println("Stopped Tuning");
				}
				if(!this->pidTuning){
					this->kp=this->pidAuto->GetKp();
					this->ki=this->pidAuto->GetKi();
					this->kd=this->pidAuto->GetKd();
					this->pid->SetTunings(this->kp,this->ki,this->kd);
					this->AutoTuneHelper(false);
				}
			}else{
				this->pid->Compute();
			}
			
			if(millis()-this->windowLastTime>this->WindowSize){
				this->windowLastTime+=this->WindowSize;
			}
			if(this->pidOutput<millis()-this->windowLastTime){
				this->output.low();
			}else{
				this->output.high();
			}
		}else{
			this->output.low();
		}
	}

	HeaterResult Read(){
		this->temperature=this->ntc.Read();
		auto max=this->tempSetPoint+(this->tempSetPoint*this->tempDeviation);
		auto min=this->tempSetPoint-(this->tempSetPoint*this->tempDeviation);
		this->result.temperature=temperature;
		this->tempOk=(this->temperature<=max && this->temperature>=min);
		this->result.state=this->output.pinState();
		return this->result;
	}

	HeaterResult GetHeaterResult(){
		return this->result;
	}

	void ChangeSetpoint(int setPoint) {
		this->tempSetPoint = setPoint;
	}

	void AutoTuneHelper(boolean start){
		if(start){
			this->ATuneModeRemember=this->pid->GetMode();
		}else{
			this->pid->SetMode(this->ATuneModeRemember);
		}
	}

	void ChangeAutoTune(){
		if(!this->pidTuning){
			this->pidOutput=this->aTuneStartValue;
			this->pidAuto->SetNoiseBand(this->aTuneNoise);
			this->pidAuto->SetOutputStep(this->aTuneStep);
			this->pidAuto->SetLookbackSec((int)this->aTuneLookBack);
			this->AutoTuneHelper(true);
			this->pidTuning=true;
		}else{
			this->pidAuto->Cancel();
			this->pidTuning=false;
			this->AutoTuneHelper(false);
		}
	}

	PID* GetPid(){
		return this->pid;
	}

private:
	TemperatureSensor 	ntc;
	DigitalOutput 		output;
	HeaterState 		heaterState=HeaterState::Off;
	PID  				*pid;
	PID_ATune			*pidAuto;
	HeaterResult		result;
	double tempDeviation;
	double tempSetPoint;
	double kp, ki, kd;
	unsigned long windowLastTime,WindowSize;
	double pidOutput=0;
	double temperature=0;
	bool tempOk = false;
	bool pidTuning=false;
	byte ATuneModeRemember=2;
	unsigned int aTuneLookBack=1;
	double aTuneStep=100, aTuneNoise=1, aTuneStartValue=500;
	
	void privateLoop() {  
		this->ProcessPid();
	}
};
*/


