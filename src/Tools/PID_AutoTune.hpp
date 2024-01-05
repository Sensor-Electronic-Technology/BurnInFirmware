#pragma once


class PID_AutoTune{
public:

    PID_AutoTune(double *in,double *out,double sp,unsigned long period,int cyc=10){
        this->input=in;
        this->outputValue=out;
        this->setPoint=sp;
        this->sampleTime=period;
        this->cycles=cyc;
    }

    void SetOutputRange(double _min,double _max){
        this->maxOut=_max;
        this->minOut=_min;
    }


    void StartTuning(){
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

    bool Tune(){
        auto now=micros();
        if(now-lastTime>=sampleTime){
            double in=*input;
            double out=*outputValue;
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
                double ku=(4.0*((maxOut-minOut)/2.0))/(M_PI*(max-min)/2.0);
                double tu=tLow+tHigh;

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
                double kpConstant = 0.2;
                double tiConstant = 0.5;
                double tdConstant = 0.33;

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

    // double GetKpRunning(){
    //     return pAvg/(cycleCount-1);
    // }
    // double GetKdRunning(){
    //     return dAvg/(cycleCount-1);
    // }
    // double GetkiRunning(){
    //     return iAvg/(cycleCount-1);
    // }
    
    double GetKp(){
        return this->kp;
    }

    double GetKd(){
        return this->kd;
    }

    double GetKi(){
        return this->ki;
    }

    bool Finished(){
        return this->cycleCount>=this->cycles;
    }


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