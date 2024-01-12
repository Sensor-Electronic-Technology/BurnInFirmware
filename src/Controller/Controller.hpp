#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include "../Serializable.hpp"
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "../Communication/ComHandler.hpp"
#include "../Logging/StationLogger.hpp"
#include "State.hpp"
#include "CurrentSelector.hpp"
#include "BurnInTimer.hpp"
#include "../constants.h"

using namespace components;

 struct TestTracker:public Serializable{
    bool running=false,paused=false;
    BurnInTimer testTimer;
    Array<BurnInTimer,PROBE_COUNT> probeTimers;

    virtual void Serialize(JsonDocument *doc,bool initialize)override{
        (*doc)[F("Running")]=this->running;
        (*doc)[F("Paused")]=this->paused;
        if(initialize){
            JsonObject timerDataJson=(*doc)[F("TestTimer")].to<JsonObject>();
            JsonArray probeTimersJson=(*doc)[F("ProbeTimers")].to<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                JsonObject probeTimerJson=probeTimersJson.add<JsonObject>();
                this->probeTimers[i].Serialize(&probeTimerJson);
            }
        }else{
            JsonObject timerDataJson=(*doc)[F("TestTimer")].as<JsonObject>();
            testTimer.Serialize(&timerDataJson);
            JsonArray probeTimersJson=(*doc)[F("ProbeTimers")].as<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                JsonObject probeTimerJson=probeTimersJson[i].as<JsonObject>();
                this->probeTimers[i].Serialize(&probeTimerJson);
            }
        }
    }

    virtual void Deserialize(JsonDocument& doc) override{
        this->running=doc[F("Running")];
        this->paused=doc[F("Paused")];
        JsonObject timerDataJson=doc[F("TestTimer")].as<JsonObject>();
        this->testTimer.Deserialize(timerDataJson);
        JsonArray probeTimersJson=doc[F("ProbeTimers")].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            JsonObject probeTimerJson=probeTimersJson[i].as<JsonObject>();
            this->probeTimers[i].Deserialize(probeTimerJson);
        }
    }

    virtual void Deserialize(JsonObject &packet)=0;
    virtual void Serialize(JsonObject *packet,bool initialize)=0;
};


class Controller:public Component{
public:
    Controller();

    //Init
    void LoadConfigurations();
    void SetupComponents();

    //actions
    void StartTest();
    void ToggelHeaters();
    void CycleCurrent();
    void StopTest();
    void RunTestProbes();
    void RunAutoTune();

    //States
    void RunningTest();
    void RunningProbeTest();

    //Modes
    void NormalRun();
    void TuningRun();

    //other
    void HandleCommand(StationCommand command);

private:
    ProbeController*    probeControl;
    HeaterController*   heaterControl;
    BurnInTimer*        burnTimer;
    CurrentSelector*    currentSelector; 
    CommandCallback     _commandCallback=[](StationCommand){};
    //Task task,nextTask;
    //typedef void(Controller::*StateHandlers)(void);
    //StateHandlers state_handler[6];
    void privateLoop() override;
};