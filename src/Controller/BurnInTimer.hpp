#pragma once
//#include <ArduinoComponents.h>
#include "../constants.h"
#include "ControllerConfiguration.hpp"


//using namespace components;

typedef struct TimerData{
    bool            running=false,paused=false;
    unsigned long   elapsed_secs=0;
    unsigned long   lastCheck=0;
    unsigned long   duration_secs=0;
}TimerData;



class BurnInTimer{
private:  
    TimerData td;
    unsigned long durSec60mA;
    unsigned long durSec120mA;
    unsigned long durSec150mA;
public:

    BurnInTimer(const BurnTimerConfig& config)
        :durSec60mA(config.time60mASecs),
        durSec120mA(config.time120mASecs),
        durSec150mA(config.time150mASecs){

    }

    void Start(CurrentValue current){
        if(!this->td.running && !this->td.paused){
            switch(current){
                case CurrentValue::c060:{
                    this->td.duration_secs=this->durSec60mA;
                    break;
                }
                case CurrentValue::c120:{
                    this->td.duration_secs=this->durSec120mA;
                    break;
                }
                case CurrentValue::c150:{
                    this->td.duration_secs=this->durSec150mA;
                    break;
                }
            }
            this->td.elapsed_secs=0;
            this->td.running=true;
            this->td.paused=false;
        }
    }
   
    void StartFrom(CurrentValue current,unsigned long elap){
        if(!this->td.running && !this->td.paused){
            switch(current){
                case CurrentValue::c060:{
                    this->td.duration_secs=this->durSec60mA;
                    break;
                }
                case CurrentValue::c120:{
                    this->td.duration_secs=this->durSec120mA;
                    break;
                }
                case CurrentValue::c150:{
                    this->td.duration_secs=this->durSec150mA;
                    break;
                }
            }
            this->td.elapsed_secs=elap;
            this->td.running=true;
            this->td.paused=false;
        }
    }
   
    void Stop(){
        this->td.running=false;
        this->td.paused=false;
        this->td.lastCheck=0; 
        this->td.elapsed_secs=0;
        this->td.duration_secs=0;
    }

    void Reset(){
        this->td.running=false;
        this->td.paused=false;
        this->td.lastCheck=false;
        this->td.elapsed_secs=0;
        this->td.duration_secs=0;
    }

    void Pause(){
        if(this->td.running && !this->td.paused){
            this->td.paused=true;
        }
    }

    void Continue(){
        if(this->td.running && this->td.paused){
            this->td.paused=false;
        }
    }

    bool IsDone(){
        return !this->td.running;
    }
    
    unsigned long GetElapsed(){
        return this->td.elapsed_secs;
    }
    
    void Increment(){
        if(this->td.running && !this->td.paused){
            auto millis_t=millis();
            if(millis_t-this->td.lastCheck>=(TIMER_PERIOD*TIMER_FACTOR)){
                this->td.lastCheck=millis_t;
                this->td.elapsed_secs++;
                bool done=(this->td.elapsed_secs*TIMER_PERIOD)>=this->td.duration_secs;
                this->td.running=!done;
            }
        }
    }

    BurnInTimer operator++(){
        this->Increment();
        return *this;
    }
};