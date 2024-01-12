#include "BurnInTimer.hpp"

    BurnInTimer::BurnInTimer(const BurnTimerConfig& config)
        :durSec60mA(config.time60mASecs),
        durSec120mA(config.time120mASecs),
        durSec150mA(config.time150mASecs){

    }

    void BurnInTimer::Start(CurrentValue current){
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
   
    void BurnInTimer::StartFrom(CurrentValue current,unsigned long elap){
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
   
    void BurnInTimer::Stop(){
        this->td.running=false;
        this->td.paused=false;
        this->td.lastCheck=0; 
        this->td.elapsed_secs=0;
        this->td.duration_secs=0;
    }

    void BurnInTimer::Reset(){
        this->td.running=false;
        this->td.paused=false;
        this->td.lastCheck=false;
        this->td.elapsed_secs=0;
        this->td.duration_secs=0;
    }

    void BurnInTimer::Pause(){
        if(this->td.running && !this->td.paused){
            this->td.paused=true;
        }
    }

    void BurnInTimer::Continue(){
        if(this->td.running && this->td.paused){
            this->td.paused=false;
        }
    }

    bool BurnInTimer::IsDone(){
        return !this->td.running;
    }
    
    unsigned long BurnInTimer::GetElapsed(){
        return this->td.elapsed_secs;
    }
    
    void BurnInTimer::Increment(){
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