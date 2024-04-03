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
    heaterState(HeatState::Off),
    mode(HeaterMode::HEATING){
    this->tempSetPoint=DEFAULT_TEMPSP;
    //this->pid=new PID(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),15);
    //this->autoTuner=new PID_AutoTune(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),15);
    this->autoTuner.SetOutputRange(0,this->WindowSize);
    // Serial.print("Sample Time=");Serial.print(this->pid.GetSampleTime());
}

Heater::Heater(){
    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);
    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),15);
    //this->autoTuner=new PID_AutoTune(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),15);
    this->autoTuner.SetOutputRange(0,this->WindowSize);
}

void Heater::SetConfiguration(const HeaterConfig& config){
    Serial.println(F("Heater Configuration Set"));
    this->id=config.HeaterId;
    this->ntc.Setup(config.ntcConfig);
    this->output.SetPinNumber(config.Pin);
    this->kp=config.pidConfig.kp;
    this->ki=config.pidConfig.ki;
    this->kd=config.pidConfig.kd;
    this->tempDeviation=config.tempDeviation;
    this->WindowSize=config.pidConfig.windowSize;
    this->tempSetPoint=DEFAULT_TEMPSP;

    this->mode=HeaterMode::HEATING;
    this->heaterState=HeatState::Off;

    this->pid.Setup(&this->temperature,&this->pidOutput,&this->tempSetPoint,this->kp,this->ki,this->kd);
    this->pid.SetTuning(this->kp,this->ki,this->kd);
    this->pid.SetOutputRange(0,this->WindowSize,true);

    this->autoTuner.Setup(&this->temperature,&this->pidOutput,this->tempSetPoint,this->pid.GetSampleTime(),15);
    this->autoTuner.SetOutputRange(0,this->WindowSize);


    Serial.println(F("End of Heater Configuration Set"));
    // Serial.print("Sample Time=");Serial.print(this->pid.GetSampleTime());
}

void Heater::BuildStateMachine(){


}

void Heater::Initialize(){
    this->output.low();
}

void Heater::UpdatePid(HeaterTuneResult newPid){
    this->pid.SetTuning(newPid.kp,newPid.ki,newPid.kd);
}

void Heater::TurnOn(){
    this->heaterState=HeatState::On;
    this->windowLastTime=millis();
    this->pid.Start();
}

void Heater::TurnOff(){
    this->heaterState=HeatState::Off;
}

void Heater::StartTuning(){
    this->autoTuner.StartTuning();
    this->isTuning=true;
}

void Heater::StopTuning(){
    this->isTuning=false;
    this->PrintTuning(false);
}

void Heater::SwitchMode(HeaterMode nextMode){

    if(this->mode!=nextMode){
        switch(this->mode){
            //Transition to Tuning
            case HeaterMode::HEATING:{
                this->TurnOff();
                this->mode=nextMode;
                break;
            }
            //Transition to PID(Normal Operation)
            case HeaterMode::ATUNE:{
                this->StopTuning();
                this->TurnOff();
                this->mode=nextMode;
                break;
            }
        }
    }
}

void Heater::PrintTuning(bool completed){
    if(completed){
        HeaterTuneResult result;
        result.kd=this->autoTuner.GetKd();
        result.ki=this->autoTuner.GetKi();
        result.kp=this->autoTuner.GetKp();
        result.complete=completed;
        result.heaterNumber=this->id;
        this->tuningCompleteCb(result);
        StationLogger::Log(LogLevel::INFO,true,false,F("Heater %d Tuning Complete"),this->id);
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

void Heater::RunPid(){
    if(this->nextState.pidState!=this->state.pidState){
        switch(nextState.pidState){
            case HeaterState::WARMUP:{
                // if(state.pidState==PIDState::OFF){
                //     this->pid.Start();
                // }
                break;
            }
            case HeaterState::ON:{
                // if(state.pidState==PIDState::WARMUP){
                //     this->pid.Start();
                // }
                break;
            }
            case HeaterState::OFF:{
                //this->pid.Stop();
                break;
            }
        }
        this->state=nextState;
    }
    auto now=millis();
    this->Read();
    // if(this->heaterState==HeaterState::On){
    //     if(this->pid.Run()){
    //         this->windowLastTime=now;
    //     }
    //     this->OutputAction(now);
    // }else{
    //     this->output.low();
    // }
}

void Heater::RunAutoTune(){
    if(state.tuneState!=nextState.tuneState){
        switch(nextState.tuneState){
            case TuneState::TUNE_IDLE:{
                if(state.tuneState==TuneState::TUNE_RUNNING){
                    this->StopTuning();
                }else if(state.tuneState==TuneState::TUNE_COMPLETE){
                    this->PrintTuning(true);
                }
                break;
            }
            case TuneState::TUNE_RUNNING:{
                this->StartTuning();
                break;
            }
            case TuneState::TUNE_COMPLETE:{
                this->PrintTuning(true);
                break;
            }
        }
        this->state=nextState;
    }

    auto now=millis();
    this->Read();
    if(this->isTuning){
        if(this->autoTuner.Tune()){
            if(!this->relayState){
                this->relayState=true;
                this->output.high();
                //Serial.print("H");Serial.print("[F(");Serial.print(this->id);Serial.print(")]: ");
                //Serial.print("Output high");Serial.print(" Pid Out: ");Serial.println(pidOutput);
            }
        }else{
            if(this->relayState){
                this->relayState=false;
                this->output.low();
                //Serial.print("H");Serial.print("[F(");Serial.print(this->id);Serial.print(")]: ");
                //Serial.print("Output Low");Serial.print(" Pid Out: ");Serial.println(pidOutput);
            }
        }
        this->OutputAction(now);
        if(this->autoTuner.Finished()){
            this->isTuning=false;
            HeaterTuneResult result;
            result.kd=this->autoTuner.GetKd();
            result.ki=this->autoTuner.GetKi();
            result.kp=this->autoTuner.GetKp();
            result.ki=true;
            result.heaterNumber=this->id;
            this->tuningCompleteCb(result);
            //this->PrintTuning(true); //debugging
        }
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
    HeaterResult data=this->result;
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
}