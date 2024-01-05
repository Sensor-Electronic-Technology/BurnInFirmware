#pragma once


class PID{
public:
    PID(double* in,double *out,double *set,double _kp,double _ki,double _kd){
        this->input=in;
        this->output=out;
        this->setpoint=set;
        this->sampleTime=1e5;
        this->SetTuning(_kp,_ki,_kd);
        this->lastTime=micros()-this->sampleTime;

    }

    void SetTuning(double _kp,double _ki,double _kd){
        if(_kp<0 || _ki<0 || _kd<0){
            return;
        }
        if(ki==0){
            this->outputSum=0;
        }
        double sampleSec=this->sampleTime/1e6;
        this->dispKd=_kd;
        this->dispKp=_kp;
        this->dispKi=_ki;
        this->kp=_kp;
        this->ki=_ki*sampleSec;
        this->kd=_kd/sampleSec;
    }

    void SetOutputRange(double _min,double _max,bool relayCntrl){
        if(_min>=_max){
            return;
        }
        this->max=_max;
        this->min=_min;
        *this->output=constrain(*this->output,this->min,this->max);
        if(relayCntrl){
            this->SetSampleTime(_max*1000);
        }
    }

    void SetSampleTime(unsigned long newSampleTime){
        if(newSampleTime>0){
            double ratio=newSampleTime/this->sampleTime;
            this->ki*=ratio;
            this->kd/=ratio;
            this->sampleTime=newSampleTime;
        }
    }
    
    unsigned long GetSampleTime(){
        return this->sampleTime;
    }

    void Start(){
        this->outputSum=*this->output;
        this->lastInput=*this->input;
        this->outputSum=constrain(this->outputSum,this->min,this->max);
    }

    void Reset(){
        this->lastTime=micros()-this->sampleTime;
        this->lastInput=0;
        this->outputSum=0;
        this->pTerm=0;
        this->iTerm=0;
        this->dTerm=0;
    }
    
    bool Run(){
        auto now=micros();
        auto dt=now-lastTime;
        if(dt>=sampleTime){
            double in,din,dErr;
            double peTerm,pmTerm;
            double iTermOut;

            in=*input;
            din=in-lastInput;
            error=*setpoint-in;
            dErr=error-lastError;

            peTerm=kp*error;
            pmTerm=kp*din;
            //proportional on Error Measure
            peTerm*=0.5;
            pmTerm*=0.5;
            pTerm=peTerm-pmTerm;
            iTerm=ki*error;
            dTerm=kd*dErr;//d on error

            //anti integral windup
            bool antiWind=false;
            iTermOut=(peTerm-pmTerm)+ki*(iTerm+error);
            if(iTermOut>max && dErr>0){
                antiWind=true;
            }else if(iTermOut<min && dErr<0){
                antiWind=true;
            }
            if(antiWind && ki){
                iTerm=constrain(iTermOut,-max,max);
            }

            outputSum+=iTerm;
            outputSum=constrain(outputSum-pmTerm,min,max);
            *output=constrain(outputSum+peTerm+dTerm,min,max);

            lastError=error;
            lastInput=in;
            lastTime=now;
            return true;
        }
        return false;
    }

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