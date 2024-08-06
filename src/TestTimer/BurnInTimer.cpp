#include "BurnInTimer.hpp"

    BurnInTimer::BurnInTimer(const BurnTimerConfig& config)
        :durSec60mA(config.time60mASecs),
        durSec120mA(config.time120mASecs),
        durSec150mA(config.time150mASecs),
        timeOffPercent(config.timeOffPercent){
    }

    void BurnInTimer::SetConfig(const BurnTimerConfig& config){
        this->durSec60mA=config.time60mASecs;
        this->durSec120mA=config.time120mASecs;
        this->durSec150mA=config.time150mASecs;
        this->timeOffPercent=config.timeOffPercent;
    }

    bool BurnInTimer::Start(CurrentValue current){
        if(!this->testTimer.running){
            switch(current){
                case CurrentValue::c060:{
                    this->testTimer.duration_secs=this->durSec60mA;
                    //this->testTimer.duration_secs=60;
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
            for(uint8_t i=0;i<PROBE_COUNT;i++){
                this->testTimer.probeRunTimes[i]=0ul;
                this->probeRunTimeOkay[i]=true;
            }
            this->testTimer.elapsed_secs=0;
            this->testTimer.running=true;
            this->testTimer.paused=false;
            this->minTimeOn=(this->testTimer.duration_secs*(this->timeOffPercent/100.0f));
            
            this->testTimer.lastCheck=millis();
            return true;
        }
        return false;
    }

    bool BurnInTimer::Start(const TimerData& savedState){
        this->testTimer=savedState;
        this->testTimer.running=true;
        this->testTimer.paused=false;
        this->minTimeOn=(this->testTimer.duration_secs*(this->timeOffPercent/100.0f));
        this->testTimer.lastCheck=millis();
        return true;
    }

    void BurnInTimer::Stop(){
        this->testTimer.running=false;
        this->testTimer.paused=false;
    }

    void BurnInTimer::Reset(){
        this->testTimer.Reset();
    }

    void BurnInTimer::Pause(){
        // if(this->testTimer.running && !this->testTimer.paused){
        //     this->testTimer.paused=true;
        // }
        this->testTimer.paused=true;
    }

    void BurnInTimer::Continue(){
        // if(this->testTimer.running && this->testTimer.paused){
        //     this->testTimer.paused=false;
        // }
        this->testTimer.paused=false;
    }

    bool BurnInTimer::IsDone(){
        return !this->testTimer.running;
    }

    bool BurnInTimer::IsPaused(){
        return this->testTimer.paused;
    }

    bool BurnInTimer::IsRunning(){
        return this->testTimer.running && !this->testTimer.paused;
    }
    
    unsigned long BurnInTimer::GetElapsed(){
        return this->testTimer.elapsed_secs;
    }

    const TimerData& BurnInTimer::GetTimerData(){
        return this->testTimer;
    }
    
    void BurnInTimer::Increment(bool probeOkay[PROBE_COUNT]){
        if(this->testTimer.running && !this->testTimer.paused){
            auto millis_t=millis();
            if(millis_t-this->testTimer.lastCheck>=(TIMER_PERIOD*TIMER_FACTOR)){
                this->testTimer++;
                for(uint8_t i=0;i<PROBE_COUNT;i++){
                    if(probeOkay[i]){
                        this->testTimer.incrementProbe(i);
                    }
                    auto remaining=this->testTimer.Remaining();
                    if(this->testTimer.probeRunTimes[i]+remaining<=this->minTimeOn){
                        probeRunTimeOkay[i]=false;
                    }else{
                        probeRunTimeOkay[i]=probeOkay[i];
                    }
                }
                this->testTimer.lastCheck=millis();
                if(this->testTimer.is_done()){
                    this->_finishedCallback();
                }
            }
        }
    }