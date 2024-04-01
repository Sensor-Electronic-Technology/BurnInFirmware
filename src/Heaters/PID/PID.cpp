
#include "PID.hpp"

/**
 * @brief Construct a new PID::PID object default constructor
 * 
 * @param in pointer to the input variable.  i.e temperature value
 * @param out pointer to output variable, i.e analog output, duty cycle, etc
 * @param set setpoint
 * @param _kp proportional gain
 * @param _ki integral gain
 * @param _kd derivitive gain
 */

PID::PID(double* in,double *out,double *set,double _kp,double _ki,double _kd){
    this->input=in;
    this->output=out;
    this->setpoint=set;
    this->sampleTime=1e5;
    this->SetTuning(_kp,_ki,_kd);
    this->lastTime=micros()-this->sampleTime;

}

void PID::Setup(double* in,double *out,double *set,double _kp,double _ki,double _kd){
    this->input=in;
    this->output=out;
    this->setpoint=set;
    this->sampleTime=1e5;
    this->SetTuning(_kp,_ki,_kd);
    this->lastTime=micros()-this->sampleTime;
}

/**
 * @brief Sets the gains.  Can be used at anytime
 * 
 * @param _kp proportional gain
 * @param _ki integral gain
 * @param _kd derivitive gain
 */
void PID::SetTuning(double _kp,double _ki,double _kd){
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

/**
 * @brief Sets the output range the output variable will be between
 * 
 * @param _min min output value
 * @param _max max output value
 * @param relayCntrl whether out not this is relay output. If it is the sample time is set to the max x1000
 * This implies the output range is a time window
 */
void PID::SetOutputRange(double _min,double _max,bool relayCntrl){
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

/**
 * @brief Update the sample time
 * 
 * @param newSampleTime new sample time
 */
void PID::SetSampleTime(unsigned long newSampleTime){
    if(newSampleTime>0){
        double ratio=newSampleTime/this->sampleTime;
        this->ki*=ratio;
        this->kd/=ratio;
        this->sampleTime=newSampleTime;
    }
}

/**
 * @brief Returns the sample time
 * 
 * @return unsigned long,the sample time
 */
unsigned long PID::GetSampleTime(){
    return this->sampleTime;
}

/**
 * @brief Starts the PID controller.  Resets variables
 * 
 */
void PID::Start(){
    this->outputSum=*this->output;
    this->lastInput=*this->input;
    this->outputSum=constrain(this->outputSum,this->min,this->max);
}

/**
 * @brief Resets all variables
 * 
 */
void PID::Reset(){
    this->lastTime=micros()-this->sampleTime;
    this->lastInput=0;
    this->outputSum=0;
    this->pTerm=0;
    this->iTerm=0;
    this->dTerm=0;
}

/**
 * @brief Runs the PID controller
 *  the return value indicates the output value has been modified
 *  If using a relay then reset time variable, if is analog output then set the output
 * @return true  
 * @return false 
 */
bool PID::Run(){
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