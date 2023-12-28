#pragma once
#include <ArduinoComponents.h>
#include "util.hpp"
#include "ntc.hpp"
#include "PID_AutoTune_v0.h"
#include "heater.hpp"
#include <PID_v1.h>

using namespace components;
#define PIDVERSION 		2
#define KP_DEFAULT		2
#define KI_DEFAULT		5
#define KD_DEFAULT		1
#define Window_Default	500

class heater_pid_tune{

public:

    heater_pid_tune(Heater* heater){
        this->heater=heater;
        this->pid=this->heater->GetPid();
    }

    void Setup(){
        this->autoTuner=new PID_ATune(&this->temperature,&this->output);
        //should I use heater instead of manual PID control? Probably
        //setup PID
        //where should I get the setp from
    }

    void ChangeAutoTune(){
        if(!tuning)
        {
            //Set the output to the desired starting frequency.
            this->output=this->tuneStart;;
            this->autoTuner->SetNoiseBand(this->tuneNoise);
            this->autoTuner->SetOutputStep(this->tuneStep);
            this->autoTuner->SetLookbackSec((int)this->tuneLookBack);
            this->TuneHelper(true);
            this->tuning = true;
        }else{ //cancel autotune
            this->autoTuner->Cancel();
            tuning = false;
            this->TuneHelper(false);
        }
    }

    void HandlerSerial(){
        if(Serial.available())
        {
            char b = Serial.read(); 
            Serial.flush(); 
            if((b=='1' && !this->tuning) || (b!='1' && this->tuning))
                this->ChangeAutoTune();
        }
    }

    void SerialSend(){
        // Serial.print("setpoint: ");Serial.print(setpoint); Serial.print(" ");
        // Serial.print("input: ");Serial.print(input); Serial.print(" ");
        // Serial.print("output: ");Serial.print(output); Serial.print(" ");
        // if(tuning){
        //     Serial.println("tuning mode");
        // } else {
        //     Serial.print("kp: ");Serial.print(pid.GetKp());Serial.print(" ");
        //     Serial.print("ki: ");Serial.print(pid.GetKi());Serial.print(" ");
        //     Serial.print("kd: ");Serial.print(pid.GetKd());Serial.println();
        // }
    }

    void Run(){
        this->temperature=this->heater->ReadTempManual();
        if(tuning){
            byte val = (this->autoTuner->Runtime());
            if (val!=0){
                tuning = false;
            }
        }
        if(!tuning){ //we're done, set the tuning parameters
            this->heaterConfig.kp = this->autoTuner->GetKp();
            this->heaterConfig.ki = this->autoTuner->GetKi();
            this->heaterConfig.kd = this->autoTuner->GetKd();
            this->pid->SetTunings(this->heaterConfig.kp,
                                this->heaterConfig.ki,
                                this->heaterConfig.kd);
            this->TuneHelper(false);
        }else{
            this->pid->Compute();
        } 
    }

    void TuneHelper(bool start){
        if(start){
            this->tuneModeSaved = pid->GetMode();
        }else{
            pid->SetMode(this->tuneModeSaved);
        }
    }


private:
    HeaterConfig    heaterConfig;
    PID_ATune       *autoTuner;
    PID             *pid;
    Heater          *heater;

    byte tuneModeSaved=2;
    double tuneStep,tuneNoise,tuneStart;
    unsigned long tuneLookBack;
    double setPoint,temperature;
    double output=0;
    bool tuning=false;
};