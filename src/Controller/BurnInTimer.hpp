#pragma once
//#include <ArduinoComponents.h>
#include "../constants.h"
#include "ControllerConfiguration.hpp"


using namespace components;

struct TimerData{
    bool            running=false,paused=false;
    unsigned long   elapsed_secs=0;
    unsigned long   lastCheck=0;
    unsigned long   duration_secs=0;
    unsigned long   timeOnSecs=0;

    void Serialize(JsonObject* timerJson){
        (*timerJson)[F("Running")]=this->running;
        (*timerJson)[F("Paused")]=this->paused;
        (*timerJson)[F("ElapsedSecs")]=this->elapsed_secs;
        (*timerJson)[F("DurationSecs")]=this->duration_secs;
        (*timerJson)[F("TimeOnSecs")]=this->timeOnSecs;
    }

    void Deserialize(JsonObject timerJson){
        this->running=timerJson[F("Running")];
        this->paused=timerJson[F("Paused")];
        this->elapsed_secs=timerJson[F("ElapsedSecs")];
        this->duration_secs=timerJson[F("DurationSecs")];
        this->timeOnSecs=timerJson[F("TimeOnSecs")];
    }

};

class BurnInTimer{
private:  
    TimerData td;
    unsigned long durSec60mA;
    unsigned long durSec120mA;
    unsigned long durSec150mA;
public:

    BurnInTimer(const BurnTimerConfig& config);

    void Start(CurrentValue current);
   
    void StartFrom(CurrentValue current,unsigned long elap);
   
    void Stop();

    void Reset();

    void Pause();

    void Continue();

    bool IsDone();
    
    unsigned long GetElapsed();
    
    void Increment();

    void Serialize(JsonObject* timerJson){
        // (*timerJson)[F("Running")]=this->td.running;
        // (*timerJson)[F("Paused")]=this->td.paused;
        // (*timerJson)[F("ElapsedSecs")]=this->td.elapsed_secs;
        // (*timerJson)[F("DurationSecs")]=this->td.duration_secs;
        // (*timerJson)[F("TimeOnSecs")]=this->td.timeOnSecs;
        this->td.Serialize(timerJson);
    }

    void Deserialize(JsonObject timerJson){
        // this->td.running=timerJson[F("Running")];
        // this->td.paused=timerJson[F("Paused")];
        // this->td.elapsed_secs=timerJson[F("ElapsedSecs")];
        // this->td.duration_secs=timerJson[F("DurationSecs")];
        // this->td.timeOnSecs=timerJson[F("TimeOnSecs")];
        this->td.Deserialize(timerJson);
    }

    BurnInTimer& operator++(){
        this->Increment();
        return *this;
    }
};