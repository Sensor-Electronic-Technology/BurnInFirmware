#pragma once
#include <Arduino.h>
/**
 * @brief PID controller 
 * 
 * 
 * 
 */
class PID{
public:
    PID(float* in=nullptr,float *out=nullptr,float *set=nullptr,float _kp=0,float _ki=0,float _kd=0);
    void Setup(float* in,float *out,float *set,float _kp,float _ki,float _kd);
    void SetTuning(float _kp,float _ki,float _kd);
    void SetOutputRange(float _min,float _max,bool relayCntrl);
    void SetSampleTime(unsigned long newSampleTime);
    unsigned long GetSampleTime();
    void Start();
    void Reset();
    bool Run();

private:
    float max;
    float min;
    float kp,ki,kd;
    float dispKp,dispKi,dispKd;
    float pTerm,iTerm,dTerm;
    float error;
    float outputSum;
    float sampleTime;
    unsigned long lastTime;
    float lastInput;
    float lastError;

    //relay control
    unsigned long lastRelayCheck;
    bool relayStatus=false;
    unsigned long windowStart;


    float *output,*input,*setpoint;

};