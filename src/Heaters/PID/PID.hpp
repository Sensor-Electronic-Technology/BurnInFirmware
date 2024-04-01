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
    PID(double* in=nullptr,double *out=nullptr,double *set=nullptr,double _kp=0,double _ki=0,double _kd=0);
    void Setup(double* in,double *out,double *set,double _kp,double _ki,double _kd);
    void SetTuning(double _kp,double _ki,double _kd);
    void SetOutputRange(double _min,double _max,bool relayCntrl);
    void SetSampleTime(unsigned long newSampleTime);
    unsigned long GetSampleTime();
    void Start();
    void Reset();
    bool Run();

private:
    double max;
    double min;
    double kp,ki,kd;
    double dispKp,dispKi,dispKd;
    double pTerm,iTerm,dTerm;
    double error;
    double outputSum;
    double sampleTime;
    unsigned long lastTime;
    double lastInput;
    double lastError;

    //relay control
    unsigned long lastRelayCheck;
    bool relayStatus=false;
    unsigned long windowStart;


    double *output,*input,*setpoint;

};