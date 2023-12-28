#pragma once
#include <ArduinoComponents.h>
#include "util.hpp"

using namespace components;

typedef struct timer_data{
    bool            running,paused=false;
    unsigned long   elapsed_secs=0;
    unsigned long   lastCheck=0;
    unsigned long   duration_secs=0;
}timer_data;



class burn_timer{
private:  
    timer_data td;

public:
    void start(unsigned long dur){
        if(!this->td.running && !this->td.paused){
            this->td.duration_secs=dur;
            this->td.elapsed_secs=0;
            this->td.running=true;
            this->td.paused=false;
        }
    }
   
    void start_from(unsigned long dur,unsigned long elap){
        if(!this->td.running && !this->td.paused){
            this->td.duration_secs=dur;
            this->td.elapsed_secs=elap;
            this->td.running=true;
            this->td.paused=false;
        }
    }
   
    void stop(){
        this->td.running=false;
        this->td.paused=false;
        this->td.lastCheck=0; 
        this->td.elapsed_secs=0;
        this->td.lastCheck=millisTime();
    }

    void pause(){
        if(this->td.running && !this->td.paused){
            this->td.paused=true;
        }
    }

    void cont(){
        if(this->td.running && this->td.paused){
            this->td.paused=false;
        }
    }

    bool is_done(){
        return !this->td.running;
    }
    
    unsigned long get_elapsed(){
        return this->td.elapsed_secs;
    }
    
    void increment_timer(){
        if(this->td.running && !this->td.paused){
            auto millis=millisTime();
            if(millisTime()-this->td.lastCheck>=(TPeriod*TFactor)){
                this->td.lastCheck=millis;
                this->td.elapsed_secs++;
                bool done=(this->td.elapsed_secs*TPeriod)>=this->td.duration_secs;
                this->td.running=!done;
            }
        }
    }
};