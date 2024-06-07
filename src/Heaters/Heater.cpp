#include "Heater.hpp"

Heater::Heater(const HeaterConfig& config,int tempSp,unsigned long windowSize) 
    :Component(),
    id(config.HeaterId),
    ntc(config.ntcConfig),
    relayPin(config.Pin),
    kp(config.pidConfig.kp),
    kd(config.pidConfig.kd),
    ki(config.pidConfig.ki),
    tempDeviation(config.tempDeviation),
    WindowSize(windowSize),
    heaterState(HeatState::Off),
    tempSetPoint(tempSp){
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,windowSize,5);
    this->autoTuner.SetOutputRange(0,windowSize);
    pinMode(this->relayPin,OUTPUT);
    digitalWrite(this->relayPin,LOW);
    /* Debugging simu timer */
    this->timer.setTimeout([this](){
        this->isComplete=true;
        this->isTuning=false;
        HeaterTuneResult result;
        result.kd=this->kd;
        result.ki=this->ki;
        result.kp=this->kp;
        result.windowSize=this->WindowSize;
        result.complete=true;
        result.heaterNumber=this->id;
        this->tuningCompleteCb(result);
    },5000);
    
}

Heater::Heater(){
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,WindowSize,5);
    /* Debugging simu timer */
    this->timer.setTimeout([this](){
        this->isComplete=true;
        this->isTuning=false;
        HeaterTuneResult result;
        result.kd=this->kd;
        result.ki=this->ki;
        result.kp=this->kp;
        result.windowSize=this->WindowSize;
        result.complete=true;
        result.heaterNumber=this->id;
        this->tuningCompleteCb(result);
    },5000);
}

void Heater::SetConfiguration(const HeaterConfig& config,unsigned long windowSize){
    this->id=config.HeaterId;
    this->ntc.Setup(config.ntcConfig);
    this->relayPin=config.Pin;
    this->kp=config.pidConfig.kp;
    this->ki=config.pidConfig.ki;
    this->kd=config.pidConfig.kd;
    this->tempDeviation=config.tempDeviation;
    this->WindowSize=windowSize;
    this->tempSetPoint=DEFAULT_TEMPSP;
    this->heaterState=HeatState::Off;
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),5);
    this->autoTuner.SetOutputRange(0,this->pid.GetSampleTime());
    pinMode(this->relayPin,OUTPUT);
    digitalWrite(this->relayPin,LOW);

}

void Heater::Initialize(){
    digitalWrite(this->relayPin,LOW);
}

void Heater::UpdatePid(HeaterTuneResult newPid){
    this->pid.SetTuning(newPid.kp,newPid.ki,newPid.kd);
}

void Heater::TurnOn(){
    this->heaterState=HeatState::On;
    this->windowLastTime=millis();
    this->relayState=false;
    this->pid.Start();
}

void Heater::TurnOff(){
    this->heaterState=HeatState::Off;
    this->relayState=false;
    digitalWrite(this->relayPin,LOW);
}

void Heater::StartTuning(){
    this->autoTuner.StartTuning();
    this->isComplete=false;
    this->isTuning=true;
    //this->timer.start();
    
}

void Heater::StopTuning(){
    this->isTuning=false;
    this->TurnOff();
}

bool Heater::IsTuning(){
    return this->isTuning;
}

bool Heater::IsComplete(){
    return this->isComplete;
}

void Heater::RunPid(){
    auto now=millis();
    this->Read();
    if(this->heaterState==HeatState::On){
        if(this->pid.Run()){
            this->windowLastTime=now;
        }
        this->OutputAction(now);
    }else{
        digitalWrite(this->relayPin,LOW);
    }
}

void Heater::RunAutoTune(){
    this->Read();
    if(this->isTuning){
        if(this->autoTuner.Tune()){
            this->relayState=true;
            digitalWrite(this->relayPin,HIGH);
        }else{
            this->relayState=false;
            digitalWrite(this->relayPin,LOW);
        }
        if(this->autoTuner.Finished()){
            this->isComplete=true;
            this->isTuning=false;
            HeaterTuneResult result;
            result.kd=this->autoTuner.GetKd();
            result.ki=this->autoTuner.GetKi();
            result.kp=this->autoTuner.GetKp();
            result.windowSize=this->autoTuner.GetSampleTimeMs();
            result.complete=true;
            result.heaterNumber=this->id;
            this->tuningCompleteCb(result);
        }
    }
}


void Heater::OutputAction(unsigned long now){
    if(!this->relayState && this->pidOutput>(now-this->windowLastTime)){
        this->relayState=true;
        digitalWrite(this->relayPin,HIGH);
    }else if(this->relayState && this->pidOutput<(now-this->windowLastTime)){
        this->relayState=false;
        digitalWrite(this->relayPin,LOW);
    }
}

HeaterResult Heater::Read(){
    this->temperature=this->ntc.Read();
    auto max=this->tempSetPoint*(1+(this->tempDeviation/100));
    auto min=this->tempSetPoint*(1-(this->tempDeviation/100));
    HeaterResult data;
    this->result.temperature=temperature;
    this->result.tempOkay=(this->temperature<=max && this->temperature>=min);
    this->result.state=this->relayState;	
    data=result;
    return data;
}

void Heater::SetWindowSize(unsigned long windowSize){
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,windowSize,5);
}

HeaterResult Heater::GetHeaterResult(){
    HeaterResult data=this->Read();
    return data;
}

void Heater::ChangeSetpoint(int setPoint) {
    this->tempSetPoint = setPoint;
}

bool Heater::TempOkay(){
    return this->result.tempOkay;
}

void Heater::privateLoop(){
    //(this->*run[this->mode])();
    this->timer.loop();
}