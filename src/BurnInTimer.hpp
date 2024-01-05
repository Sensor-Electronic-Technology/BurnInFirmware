#pragma once
#include <ArduinoComponents.h>
#include "constants.h"

using namespace components;

typedef struct TimerData{
    bool            running,paused=false;
    unsigned long   elapsed_secs=0;
    unsigned long   lastCheck=0;
    unsigned long   duration_secs=0;
}TimerData;



class BurnInTimer{
private:  
    TimerData td;
public:
    void Start(unsigned long dur){
        if(!this->td.running && !this->td.paused){
            this->td.duration_secs=dur;
            this->td.elapsed_secs=0;
            this->td.running=true;
            this->td.paused=false;
        }
    }
   
    void StartFrom(unsigned long dur,unsigned long elap){
        if(!this->td.running && !this->td.paused){
            this->td.duration_secs=dur;
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
        this->td.lastCheck=millisTime();
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
            auto millis=millisTime();
            if(millisTime()-this->td.lastCheck>=(TIMER_PERIOD*TIMER_FACTOR)){
                this->td.lastCheck=millis;
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