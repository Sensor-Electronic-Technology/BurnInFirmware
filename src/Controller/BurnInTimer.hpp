#pragma once
//#include <ArduinoComponents.h>
#include "../constants.h"
#include "../Probes/probe_constants.h"
#include "ControllerConfiguration.hpp"

using namespace components;

struct TimerData{
    bool            running=false,paused=false;
    unsigned long   elapsed_secs=0ul;
    unsigned long   probeRunTimes[PROBE_COUNT]={0ul,0ul,0ul,0ul,0ul,0ul};
    unsigned long   lastCheck=0ul;
    unsigned long   duration_secs=0ul;
    //unsigned long   timeOnSecs=0;

    void Serialize(JsonObject* timerJson,bool initialize){
        JsonArray jsonPTimers;
        (*timerJson)[F("Running")]=this->running;
        (*timerJson)[F("Paused")]=this->paused;
        (*timerJson)[F("ElapsedSecs")]=this->elapsed_secs;
        (*timerJson)[F("DurationSecs")]=this->duration_secs;
        if(initialize){
            jsonPTimers=(*timerJson)[F("ProbeRuntimes")].to<JsonArray>();
        }else{
            jsonPTimers=(*timerJson)[F("ProbeRuntimes")].as<JsonArray>();
        }
        for(int i=0;i<PROBE_COUNT;i++){
            jsonPTimers[i]=this->probeRunTimes[i];
        }
    }

    void Deserialize(JsonObject& timerJson){
        this->running=timerJson[F("Running")];
        this->paused=timerJson[F("Paused")];
        this->elapsed_secs=timerJson[F("ElapsedSecs")];
        this->duration_secs=timerJson[F("DurationSecs")];
        //this->timeOnSecs=timerJson[F("TimeOnSecs")];
        for(int i=0;i<PROBE_COUNT;i++){
            this->probeRunTimes[i]=timerJson[F("ProbeRuntimes")][i];
        }
    }

};

class BurnInTimer{
private:  
    //TimerData testTimer;
    unsigned long durSec60mA;
    unsigned long durSec120mA;
    unsigned long durSec150mA;
public:
    TimerData testTimer;

    BurnInTimer(const BurnTimerConfig& config);

    void Start(CurrentValue current);
   
    void StartFrom(const TimerData& savedState);
   
    void Stop();

    void Reset();

    void Pause();

    void Continue();

    bool IsDone();

    bool IsPaused();
    
    bool IsRunning(){
        return this->testTimer.running || this->testTimer.paused;
    }
    
    unsigned long GetElapsed();
    
    void Increment(bool probeOkay[PROBE_COUNT]);

    void Serialize(JsonDocument* doc,bool initialize){
        JsonObject jsonTimer;
        jsonTimer=(initialize) ? (*doc)[F("BurnTimer")].to<JsonObject>():(*doc)[F("BurnTimer")].as<JsonObject>();
        this->testTimer.Serialize(&jsonTimer,initialize);
    }

    void Deserialize(JsonDocument& doc) {
        JsonObject timerJson=doc[F("BurnTimer")].as<JsonObject>();
        this->testTimer.Deserialize(timerJson);
    }

    void Serialize(JsonObject* packet,bool initialize);
    void Deserialize(JsonObject& packet);
};