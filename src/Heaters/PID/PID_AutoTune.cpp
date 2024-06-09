#include "PID_AutoTune.hpp"

    PID_AutoTune::PID_AutoTune(float *in,float *out,float sp,unsigned long period,int cyc=5){
        this->input=in;
        this->outputValue=out;
        this->setPoint=sp;
        this->sampleTime=period*1000;
        this->cycles=cyc;
    }

    void PID_AutoTune::Setup(float *in,float *out,float sp,unsigned long period,int cyc=5){
        this->input=in;
        this->outputValue=out;
        this->setPoint=sp;
        this->sampleTime=period*1000;
        this->cycles=cyc;
    }

    void PID_AutoTune::SetOutputRange(float _min,float _max){
        this->maxOut=_max;
        this->minOut=_min;
    }

    void PID_AutoTune::StartTuning(){
        this->cycleCount=0;
        this->output=true;
        *this->outputValue=this->maxOut;
        this->t1=this->t2=micros();
        this->lastTime=this->t1-this->sampleTime;
        this->tHigh=this->tLow=0;
        this->max=-1000000000000;
        this->min=1000000000000;
        this->pAvg=this->iAvg=this->dAvg=0;
    }

    int PID_AutoTune::GetSampleTimeMs(){
        return this->sampleTime/1000;
    }

    bool PID_AutoTune::Tune(){
        auto now=micros();
        if(now-lastTime>=sampleTime){
            float in=*input;
            float out=*outputValue;
            max=(max>in) ? max:in;
            min=(min<in) ? min:in;

            if(output && in>setPoint){
                output=false;
                out=minOut;
                t1=now;
                tHigh=t1-t2;
                max=setPoint;
            }

            if(!output && in<setPoint){
                output=true;
                out=maxOut;
                t2=now;
                tLow=t2-t1;

                //Caluculate Ku(gain) Ku=4d/pi*a
                //d=output amplitude, a=input amplitude
                float ku=(4.0*((maxOut-minOut)/2.0))/(M_PI*(max-min)/2.0);
                float tu=tLow+tHigh;

                //Calculate gains
                // Kp = 0.6Ku = Kc
                // Ti = 0.5Tu = Kc/Ki
                // Td = 0.125Tu = Kd/Kc
                // Solve for Kp, Ki, and Kd
                //
                // Kp = 0.6Ku
                // Ki = Kp / (0.5Tu)
                // Kd = 0.125 * Kp * Tu
                /**Ziegler–Nichols No Overshoot Constants
                 * 
                 * Kp=0.2Ku,Ti=0.5Tu,Td=0.33Tu,Ki=0.66Ku/Tu,Kd=0.11KuTu
                 * */
                float kpConstant = 0.2;
                float tiConstant = 0.5;
                float tdConstant = 0.33;

                //Calculate Gains
                kp=kpConstant*ku; 
                ki=(kp/(tiConstant*tu))*sampleTime;
                kd=(tdConstant*kp*tu)/sampleTime;

                if(cycleCount>1){
                    pAvg+=kp;
                    iAvg+=ki;
                    dAvg+=kd;
                }

                min=setPoint;
                cycleCount++;
            }

            if(cycleCount>=cycles){
                output=false;
                out=minOut;
                kp=pAvg/(cycleCount-1);
                ki=iAvg/(cycleCount-1);
                kd=dAvg/(cycleCount-1);
            }
            *outputValue=out;
            lastTime=now;
            //return output;
        }
        return output;
    }

    // float GetKpRunning(){
    //     return pAvg/(cycleCount-1);
    // }
    // float GetKdRunning(){
    //     return dAvg/(cycleCount-1);
    // }
    // float GetkiRunning(){
    //     return iAvg/(cycleCount-1);
    // }
    
    float PID_AutoTune::GetKp(){
        return this->kp;
    }

    float PID_AutoTune::GetKd(){
        return this->kd;
    }

    float PID_AutoTune::GetKi(){
        return this->ki;
    }

    bool PID_AutoTune::Finished(){
        return this->cycleCount>=this->cycles;
    }