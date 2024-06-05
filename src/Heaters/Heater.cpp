#include "Heater.hpp"

Heater::Heater(const HeaterConfig& config,int tempSp) 
    :Component(),
    id(config.HeaterId),
    ntc(config.ntcConfig),
    relayPin(config.Pin),
    kp(config.pidConfig.kp),
    kd(config.pidConfig.kd),
    ki(config.pidConfig.ki),
    tempDeviation(config.tempDeviation),
    WindowSize(config.pidConfig.windowSize),
    heaterState(HeatState::Off),
    tempSetPoint(tempSp){
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),3);
    this->autoTuner.SetOutputRange(0,this->WindowSize);
    pinMode(this->relayPin,OUTPUT);
    digitalWrite(this->relayPin,LOW);
/*     timer.setTimeout([&](){
        this->isTuning=false;
        this->isComplete=true;
        this->PrintTuning(true);
    },2000); */
    //RegisterChild(timer);
}

Heater::Heater(){
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),3);
    this->autoTuner.SetOutputRange(0,this->WindowSize);
}

void Heater::SetConfiguration(const HeaterConfig& config){
    //Serial.println(F("Heater Configuration Set"));
    this->id=config.HeaterId;
    this->ntc.Setup(config.ntcConfig);
    this->relayPin=config.Pin;
    this->kp=config.pidConfig.kp;
    this->ki=config.pidConfig.ki;
    this->kd=config.pidConfig.kd;
    this->tempDeviation=config.tempDeviation;
    this->WindowSize=config.pidConfig.windowSize;
    this->tempSetPoint=DEFAULT_TEMPSP;
    this->heaterState=HeatState::Off;
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetTuning(this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),15);
    this->autoTuner.SetOutputRange(0,this->WindowSize);
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
    Serial.println("Heater "+String(this->id)+" Started Tuning");
}

void Heater::StopTuning(){
    this->isTuning=false;
    //this->PrintTuning(false);
    Serial.println("Heater "+String(this->id)+" Stopped Tuning");
    this->TurnOff();
}

void Heater::PrintTuning(bool completed){
    if(completed){
        HeaterTuneResult result;
        // result.kd=this->autoTuner.GetKd();
        // result.ki=this->autoTuner.GetKi();
        // result.kp=this->autoTuner.GetKp();
        result.kd=this->kd;
        result.ki=this->ki;
        result.kp=this->kp;
        result.complete=completed;
        result.heaterNumber=this->id;
        //Serial.println("Sending Tuning Results for Heater "+String(this->id));
        this->tuningCompleteCb(result);
        //HeatersController notifys PC
        //StationLogger::Log(LogLevel::INFO,true,false,F("Heater %d Tuning Complete"),this->id);
        
    }else{
        #if HEATER_DEBUG==1
        Serial.print("H");Serial.print("[F(");Serial.print(this->id);Serial.print(")]: ");
        Serial.print("kp=");Serial.print(this->autoTuner.GetKp());
        Serial.print(" , ki=");Serial.print(this->autoTuner.GetKd());
        Serial.print(" , kd=");Serial.print(this->autoTuner.GetKi());
        #endif
    }
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
    auto now=millis();
    this->Read();
    if(this->isTuning){
        if(this->autoTuner.Tune()){
            this->relayState=true;
            digitalWrite(this->relayPin,HIGH);
            // if(!this->relayState){
            //     this->relayState=true;
            //     digitalWrite(this->relayPin,HIGH);
            //     #if HEATER_DEBUG==1
            //     Serial.print("H");Serial.print("[F(");Serial.print(this->id);Serial.print(")]: ");
            //     Serial.print("Output high");Serial.print(" Pid Out: ");Serial.println(pidOutput);
            //     #endif
            // }
        }else{
            this->relayState=false;
            digitalWrite(this->relayPin,LOW);
            // if(this->relayState){
            //     this->relayState=false;
            //     digitalWrite(this->relayPin,LOW);
            //     #if HEATER_DEBUG==1
            //     Serial.print("H");Serial.print("[F(");Serial.print(this->id);Serial.print(")]: ");
            //     Serial.print("Output Low");Serial.print(" Pid Out: ");Serial.println(pidOutput);
            //     #endif

            // }
        }
        //this->OutputAction(now);
        if(this->autoTuner.Finished()){
            this->isComplete=true;
            this->isTuning=false;
            HeaterTuneResult result;
            result.kd=this->autoTuner.GetKd();
            result.ki=this->autoTuner.GetKi();
            result.kp=this->autoTuner.GetKp();
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
        #if HEATER_DEBUG==1
        Serial.print("Output high Now: ");Serial.print(now);Serial.print(" Last: ");Serial.print(windowLastTime);
        Serial.print(" Pid Out: ");Serial.println(pidOutput);
        #endif
    }else if(this->relayState && this->pidOutput<(now-this->windowLastTime)){
        this->relayState=false;
        digitalWrite(this->relayPin,LOW);
        #if HEATER_DEBUG==1
        Serial.print("Output low Now: ");Serial.print(now);Serial.print(" Last: ");Serial.print(windowLastTime);
        Serial.print(" Pid Out: ");Serial.println(pidOutput);
        #endif
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
    //this->timer.loop();
}