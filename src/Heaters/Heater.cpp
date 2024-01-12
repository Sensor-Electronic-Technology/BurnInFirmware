#include "Heater.hpp"

Heater::Heater(const HeaterConfig& config) 
    :Component(),
    id(config.HeaterId),
    ntc(config.ntcConfig),
    output(config.Pin),
    kp(config.pidConfig.kp),
    kd(config.pidConfig.kd),
    ki(config.pidConfig.ki),
    tempDeviation(config.tempDeviation),
    WindowSize(config.pidConfig.windowSize),
    heaterState(HeaterState::Off),
    heaterMode(HeaterMode::PID_RUN){

    this->tempSetPoint=DEFAULT_TEMPSP;
    this->pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,
    this->kp,this->ki,this->kd);
    this->pid->SetOutputRange(0,this->WindowSize,true);
    this->autoTuner=new PID_AutoTune(&this->temperature,&this->pidOutput,this->tempSetPoint,
        this->pid->GetSampleTime(),15);
    this->run[HeaterMode::PID_RUN]=&Heater::Run;
    this->run[HeaterMode::ATUNE_RUN]=&Heater::RunAutoTune;
    this->autoTuner->SetOutputRange(0,this->WindowSize);
    Serial.print("Sample Time=");Serial.print(this->pid->GetSampleTime());
}

void Heater::Initialize(){
    this->output.low();
}

void Heater::TurnOn(){
    this->heaterState=HeaterState::On;
    this->windowLastTime=millis();
    this->pid->Start();
}

void Heater::TurnOff(){
    this->heaterState=HeaterState::Off;
}

void Heater::StartTuning(){
    this->autoTuner->StartTuning();
    this->isTuning=true;
}

void Heater::StopTuning(){
    this->isTuning=false;
    this->PrintTuning(false);
}

void Heater::SwitchMode(HeaterMode nextMode){
    if(this->heaterMode!=nextMode){
        switch(this->heaterMode){
            //Transition to Tuning
            case HeaterMode::PID_RUN:{
                this->TurnOff();
                this->heaterMode=nextMode;
                break;
            }
            //Transition to PID(Normal Operation)
            case HeaterMode::ATUNE_RUN:{
                this->StopTuning();
                this->TurnOff();
                this->heaterMode=nextMode;
                break;
            }
        }
    }
}

void Heater::RunAutoTune(){
    auto now=millis();
    this->Read();
    if(this->isTuning){
        if(this->autoTuner->Tune()){
            if(!this->relayState){
                this->relayState=true;
                this->output.high();
                Serial.print("H");Serial.print("[");Serial.print(this->id);Serial.print("]: ");
                Serial.print("Output high");Serial.print(" Pid Out: ");Serial.println(pidOutput);
            }
        }else{
            if(this->relayState){
                this->relayState=false;
                this->output.low();
                Serial.print("H");Serial.print("[");Serial.print(this->id);Serial.print("]: ");
                Serial.print("Output Low");Serial.print(" Pid Out: ");Serial.println(pidOutput);
            }
        }
        this->OutputAction(now);
        if(this->autoTuner->Finished()){
            this->isTuning=false;
            this->PrintTuning(true); //debugging
        }
    }
}

void Heater::PrintTuning(bool completed){
    Serial.print("H");Serial.print("[");Serial.print(this->id);Serial.print("]: ");
    Serial.print("kp=");Serial.print(this->autoTuner->GetKp());
    Serial.print(" , ki=");Serial.print(this->autoTuner->GetKd());
    Serial.print(" , kd=");Serial.print(this->autoTuner->GetKi());
}

bool Heater::IsTuning(){
    return this->isTuning;
}

void Heater::Run(){
    this->Read();
    if(this->heaterState==HeaterState::On){
        auto now=millis();
        if(this->pid->Run()){
            this->windowLastTime=now;
        }
        this->OutputAction(now);
    }else{
        this->output.low();
    }
}


void Heater::OutputAction(unsigned long now){
    if(!this->relayState && this->pidOutput>(now-this->windowLastTime)){
        this->relayState=true;
        this->output.high();
        #if HEATER_DEBUG==1
        Serial.print("Output high Now: ");Serial.print(now);Serial.print(" Last: ");Serial.print(windowLastTime);
        Serial.print(" Pid Out: ");Serial.println(pidOutput);
        #endif
    }else if(this->relayState && this->pidOutput<(now-this->windowLastTime)){
        this->relayState=false;
        this->output.low();
        #if HEATER_DEBUG==1
        Serial.print("Output low Now: ");Serial.print(now);Serial.print(" Last: ");Serial.print(windowLastTime);
        Serial.print(" Pid Out: ");Serial.println(pidOutput);
        #endif
    }
}

HeaterResult Heater::Read(){
    this->temperature=this->ntc.Read();
    auto max=this->tempSetPoint+(this->tempSetPoint*this->tempDeviation);
    auto min=this->tempSetPoint-(this->tempSetPoint*this->tempDeviation);
    HeaterResult data;
    this->result.temperature=temperature;
    this->result.tempOkay=(this->temperature<=max && this->temperature>=min);
    this->result.state=this->relayState;	
    data=result;
    return data;
}

HeaterResult Heater::GetHeaterResult(){
    HeaterResult data=this->result;
    return data;
}

void Heater::ChangeSetpoint(int setPoint) {
    this->tempSetPoint = setPoint;
}

void Heater::privateLoop(){
    this->run[this->heaterMode];
}