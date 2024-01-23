#include "BurnInTimer.hpp"

    BurnInTimer::BurnInTimer(const BurnTimerConfig& config)
        :durSec60mA(config.time60mASecs),
        durSec120mA(config.time120mASecs),
        durSec150mA(config.time150mASecs){
    }

    void BurnInTimer::Start(CurrentValue current){
        if(!this->testTimer.running && !this->testTimer.paused){
            switch(current){
                case CurrentValue::c060:{
                    this->testTimer.duration_secs=this->durSec60mA;
                    break;
                }
                case CurrentValue::c120:{
                    this->testTimer.duration_secs=this->durSec120mA;
                    break;
                }
                case CurrentValue::c150:{
                    this->testTimer.duration_secs=this->durSec150mA;
                    break;
                }
            }
            for(int i=0;i<PROBE_COUNT;i++){
                this->testTimer.probeRunTimes[i]=0ul;
            }
            this->testTimer.elapsed_secs=0;
            this->testTimer.running=true;
            this->testTimer.paused=false;
        }
    }
   
    void BurnInTimer::StartFrom(const TimerData& savedTimerState){
        this->testTimer=savedTimerState;
        // if(!this->testTimer.running && !this->testTimer.paused){
        //     switch(current){
        //         case CurrentValue::c060:{
        //             this->testTimer.duration_secs=this->durSec60mA;
        //             break;
        //         }
        //         case CurrentValue::c120:{
        //             this->testTimer.duration_secs=this->durSec120mA;
        //             break;
        //         }
        //         case CurrentValue::c150:{
        //             this->testTimer.duration_secs=this->durSec150mA;
        //             break;
        //         }
        //     }
        //     this->testTimer.elapsed_secs=elap;
        //     this->testTimer.running=true;
        //     this->testTimer.paused=false;
        // }
    }
   
    void BurnInTimer::Stop(){
        this->testTimer.running=false;
        this->testTimer.paused=false;
        this->testTimer.lastCheck=0; 
        this->testTimer.elapsed_secs=0;
        this->testTimer.duration_secs=0;
    }

    void BurnInTimer::Reset(){
        this->testTimer.running=false;
        this->testTimer.paused=false;
        this->testTimer.lastCheck=false;
        this->testTimer.elapsed_secs=0;
        this->testTimer.duration_secs=0;
        for(int i=0;i<PROBE_COUNT;i++){
            this->testTimer.probeRunTimes[i]=0ul;
        }
    }

    void BurnInTimer::Pause(){
        if(this->testTimer.running && !this->testTimer.paused){
            this->testTimer.paused=true;
        }
    }

    void BurnInTimer::Continue(){
        if(this->testTimer.running && this->testTimer.paused){
            this->testTimer.paused=false;
        }
    }

    bool BurnInTimer::IsDone(){
        return !this->testTimer.running;
    }

    bool BurnInTimer::IsPaused(){
        return this->testTimer.running && this->testTimer.paused;
    }
    
    unsigned long BurnInTimer::GetElapsed(){
        return this->testTimer.elapsed_secs;
    }
    
    void BurnInTimer::Increment(bool probeOkay[PROBE_COUNT]){
        if(this->testTimer.running && !this->testTimer.paused){
            auto millis_t=millis();
            if(millis_t-this->testTimer.lastCheck>=(TIMER_PERIOD*TIMER_FACTOR)){
                this->testTimer.lastCheck=millis_t;
                for(int i=0;i<PROBE_COUNT;i++){
                    if(probeOkay[i]){
                        this->testTimer.probeRunTimes[i]++;
                    }
                }
                this->testTimer.elapsed_secs++;
                bool done=(this->testTimer.elapsed_secs*TIMER_PERIOD)>=this->testTimer.duration_secs;
                this->testTimer.running=!done;
            }
        }
    }