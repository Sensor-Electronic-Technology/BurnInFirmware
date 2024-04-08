#pragma once
#include <ArduinoComponents.h>
#include "../constants.h"
#include "../Probes/probe_constants.h"
#include "../Serializable.hpp"
#include "BurnTimerConfig.hpp"

using namespace components;

struct TimerData{
    bool            running=false,paused=false;
    unsigned long   elapsed_secs=0ul;
    unsigned long   probeRunTimes[PROBE_COUNT]={0ul,0ul,0ul,0ul,0ul,0ul};
    unsigned long   lastCheck=0ul;
    unsigned long   duration_secs=0ul;
    //unsigned long   timeOnSecs=0;

    void Reset(){
        this->running=false;
        this->paused=false;
        this->lastCheck=0ul;
        this->duration_secs=0ul;
        this->elapsed_secs=0ul;
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            this->probeRunTimes[i]=0ul;
        }
    }

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
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            jsonPTimers[i]=this->probeRunTimes[i];
        }
    }

    void Deserialize(JsonObject& timerJson){
        this->running=timerJson[F("Running")];
        this->paused=timerJson[F("Paused")];
        this->elapsed_secs=timerJson[F("ElapsedSecs")];
        this->duration_secs=timerJson[F("DurationSecs")];
        //this->timeOnSecs=timerJson[F("TimeOnSecs")];
        for(uint8_t i=0;i<PROBE_COUNT;i++){
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
    TestFinsihedCallback _finishedCallback=[](){_NOP();}; 

public:
    TimerData testTimer;

    BurnInTimer(const BurnTimerConfig& config);

    BurnInTimer(){

    }

    void SetCallback(TestFinsihedCallback callback){
        this->_finishedCallback=callback;
    }

    void SetConfig(const BurnTimerConfig& config);

    const TimerData& GetTimerData();

    bool Start(CurrentValue current);

    bool Start(const TimerData& savedState);
   
    void Stop();

    void Reset();

    void Pause();

    void Continue();

    bool IsDone();

    bool IsPaused();
    
    bool IsRunning();
    
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