#pragma once
#include <Arduino.h>

/**
 * @brief PID autuning algorithm using the ziegler nichols methode
 * Gain Constants: https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method
 * Good Explaination: https://www.allaboutcircuits.com/projects/embedded-pid-temperature-control-part-6-zieglernichols-tuning/
 */

class PID_AutoTune{
public:
    PID_AutoTune(float *in=nullptr,float *out=nullptr,float sp=0,unsigned long period=0,int cyc=10);
    void Setup(float *in,float *out,float sp,unsigned long period,int cyc=10);
    void SetOutputRange(float _min,float _max);
    void StartTuning();
    bool Tune();
    float GetKp();
    float GetKd();
    float GetKi();
    bool Finished();
private:
    float setPoint;
    float minOut,maxOut;
    float sampleTime;
    int cycles=10;

    int cycleCount;
    bool output;
    float *outputValue;
    float *input;
    float t1,t2,tHigh,tLow;
    float max,min;
    float pAvg,iAvg,dAvg;
    float kp,ki,kd;
    unsigned long lastTime;
};