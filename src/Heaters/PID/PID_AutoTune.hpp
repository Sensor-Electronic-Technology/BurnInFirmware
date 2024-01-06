#pragma once
#include <Arduino.h>

/**
 * @brief PID autuning algorithm using the ziegler nichols methode
 * Gain Constants: https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method
 * Good Explaination: https://www.allaboutcircuits.com/projects/embedded-pid-temperature-control-part-6-zieglernichols-tuning/
 */

class PID_AutoTune{
public:
    PID_AutoTune(double *in,double *out,double sp,unsigned long period,int cyc=10);
    void SetOutputRange(double _min,double _max);
    void StartTuning();
    bool Tune();
    double GetKp();
    double GetKd();
    double GetKi();
    bool Finished();
private:
    double setPoint;
    double minOut,maxOut;
    double sampleTime;
    int cycles=10;

    int cycleCount;
    bool output;
    double *outputValue;
    double *input;
    double t1,t2,tHigh,tLow;
    double max,min;
    double pAvg,iAvg,dAvg;
    double kp,ki,kd;
    unsigned long lastTime;
};