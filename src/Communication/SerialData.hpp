#pragma once
#include <ArduinoJson.h>
#include <Array.h>
#include "../Serializable.hpp"
#include "../Heaters/heaters_include.h"
#include "../Probes/probes_include.h"
#include "../TestTimer/burn_timer_includes.h"
#include "../constants.h"


class SerialDataOutput:public Serializable{
public:
     float voltages[PROBE_COUNT]={0,0,0,0,0,0};
     float currents[PROBE_COUNT]={0,0,0,0,0,0};
     float temperatures[HEATER_COUNT]={0,0,0};
     float probeRuntimes[PROBE_COUNT]={0,0,0,0,0,0};
     bool heaterStates[HEATER_COUNT]={false,false,false};
     bool probeRunTimeOkay[PROBE_COUNT]={true,true,true,true,true,true};
  
     bool running=false,paused=false;
     uint8_t currentSP=150;
     float temperatureSP=85;
     long runTimeSecs=54234;
     long elapsedSecs=12345;

/*      void Set(const Array<ProbeResult,PROBE_COUNT>& probeResults,
            const Array<HeaterResult,HEATER_COUNT>& heaterResults,
            const BurnInTimer& burnTimer){ */
        void Set(ProbeResult probeResults[PROBE_COUNT],
                HeaterResult heaterResults[HEATER_COUNT],
                bool probeRtOkay[PROBE_COUNT],
                const BurnInTimer& burnTimer){ 
        
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            auto result=probeResults[i];
            this->voltages[i]=result.voltage;
            this->currents[i]=result.current;
            this->probeRuntimes[i]=burnTimer.testTimer.probeRunTimes[i];
            this->probeRunTimeOkay[i]=probeRtOkay[i];
        }

        for(uint8_t i=0;i<HEATER_COUNT;i++){
            auto result=heaterResults[i];
            this->heaterStates[i]=result.state;
            this->temperatures[i]=result.temperature;
        }

        this->running=burnTimer.testTimer.running;
        this->paused=burnTimer.testTimer.paused;
        this->runTimeSecs=burnTimer.testTimer.duration_secs;
        this->elapsedSecs=burnTimer.testTimer.elapsed_secs;  
     }

     void Deserialize(JsonDocument& doc) override{
        JsonArray v_array=doc[F("Voltages")].as<JsonArray>();
        JsonArray c_array=doc[F("Currents")].as<JsonArray>();
        JsonArray t_array=doc[F("Temperatures")].as<JsonArray>();
        JsonArray rt_array=doc[F("ProbeRuntimes")].as<JsonArray>();
        JsonArray h_array=doc[F("HeaterStates")].as<JsonArray>();
        JsonArray rtOkay_array=doc[F("ProbeRunTimeOkay")].as<JsonArray>();
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            this->voltages[i]=v_array[i];
            this->currents[i]=c_array[i];
            this->probeRuntimes[i]=rt_array[i];
            this->probeRunTimeOkay[i]=rtOkay_array[i];
            if(i<HEATER_COUNT){
                this->temperatures[i]=t_array[i];
                this->heaterStates[i]=h_array[i];
            }
        }
        this->currentSP=doc[F("CurrentSetPoint")];
        this->temperatureSP=doc[F("TemperatureSetPoint")];
        this->runTimeSecs=doc[F("RuntimeSeconds")];
        this->elapsedSecs=doc[F("ElapsedSeconds")];
        this->running=doc[F("Running")];
        this->paused=doc[F("Paused")];
     }

    void Deserialize(JsonObject& packet) override{
        JsonArray v_array=packet[F("Voltages")].as<JsonArray>();
        JsonArray c_array=packet[F("Currents")].as<JsonArray>();
        JsonArray t_array=packet[F("Temperatures")].as<JsonArray>();
        JsonArray rt_array=packet[F("ProbeRuntimes")].as<JsonArray>();
        JsonArray h_array=packet[F("HeaterStates")].as<JsonArray>();
        JsonArray rtOkay_array=packet[F("ProbeRunTimeOkay")].as<JsonArray>();
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            this->voltages[i]=v_array[i];
            this->currents[i]=c_array[i];
            this->probeRuntimes[i]=rt_array[i];
            this->probeRunTimeOkay[i]=rtOkay_array[i];
            if(i<HEATER_COUNT){
                this->temperatures[i]=t_array[i];
                this->heaterStates[i]=h_array[i];
            }
        }
        this->currentSP=packet[F("CurrentSetPoint")];
        this->temperatureSP=packet[F("TemperatureSetPoint")];
        this->runTimeSecs=packet[F("RuntimeSeconds")];
        this->elapsedSecs=packet[F("ElapsedSeconds")];
        this->running=packet[F("Running")];
        this->paused=packet[F("Paused")];
     }
     
     void Serialize(JsonDocument* doc,bool initialize) override{
        JsonArray v_array;
        JsonArray c_array;
        JsonArray t_array;
        JsonArray rt_array;
        JsonArray h_array;
        JsonArray rtOkay_array;
        if(initialize){
            v_array=(*doc)[F("Voltages")].to<JsonArray>();
            c_array=(*doc)[F("Currents")].to<JsonArray>();
            t_array=(*doc)[F("Temperatures")].to<JsonArray>();
            rt_array=(*doc)[F("ProbeRuntimes")].to<JsonArray>();
            h_array=(*doc)[F("HeaterStates")].to<JsonArray>();
            rtOkay_array=(*doc)[F("ProbeRunTimeOkay")].to<JsonArray>();
        }else{
            v_array=(*doc)[F("Voltages")].as<JsonArray>();
            c_array=(*doc)[F("Currents")].as<JsonArray>();
            t_array=(*doc)[F("Temperatures")].as<JsonArray>();
            rt_array=(*doc)[F("ProbeRuntimes")].as<JsonArray>();
            h_array=(*doc)[F("HeaterStates")].as<JsonArray>();
            rtOkay_array=(*doc)[F("ProbeRunTimeOkay")].as<JsonArray>();
        }
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            v_array[i]=this->voltages[i];
            c_array[i]=this->currents[i];
            rt_array[i]=this->probeRuntimes[i];
            rtOkay_array[i]=this->probeRunTimeOkay[i];
            if(i<HEATER_COUNT){
                t_array[i]=this->temperatures[i];
                h_array[i]=this->heaterStates[i];
            }
        }
        (*doc)[F("CurrentSetPoint")] = this->currentSP;
        (*doc)[F("TemperatureSetPoint")] = this->temperatureSP;
        (*doc)[F("RuntimeSeconds")] = this->runTimeSecs;
        (*doc)[F("ElapsedSeconds")] = this->elapsedSecs;
        (*doc)[F("Running")] = this->running;
        (*doc)[F("Paused")] = this->paused;
        //serializeJson(*doc, Serial);
     }

     void Serialize(JsonObject *packet,bool initialize) override{
        JsonArray v_array;
        JsonArray c_array;
        JsonArray t_array;
        JsonArray rt_array;
        JsonArray h_array;
        JsonArray rtOkay_array;
        if(initialize){
            v_array=(*packet)[F("Voltages")].to<JsonArray>();
            c_array=(*packet)[F("Currents")].to<JsonArray>();
            t_array=(*packet)[F("Temperatures")].to<JsonArray>();
            rt_array=(*packet)[F("ProbeRuntimes")].to<JsonArray>();
            h_array=(*packet)[F("HeaterStates")].to<JsonArray>();
            rtOkay_array=(*packet)[F("ProbeRunTimeOkay")].to<JsonArray>();
        }else{
            v_array=(*packet)[F("Voltages")].as<JsonArray>();
            c_array=(*packet)[F("Currents")].as<JsonArray>();
            t_array=(*packet)[F("Temperatures")].as<JsonArray>();
            rt_array=(*packet)[F("ProbeRuntimes")].as<JsonArray>();
            h_array=(*packet)[F("HeaterStates")].as<JsonArray>();
            rtOkay_array=(*packet)[F("ProbeRunTimeOkay")].as<JsonArray>();
        }
        for(uint8_t i=0;i<PROBE_COUNT;i++){
            v_array[i]=this->voltages[i];
            c_array[i]=this->currents[i];
            rt_array[i]=this->probeRuntimes[i];
            rtOkay_array[i]=this->probeRunTimeOkay[i];
            if(i<HEATER_COUNT){
                t_array[i]=this->temperatures[i];
                h_array[i]=this->heaterStates[i];
            }
        }
        (*packet)[F("CurrentSetPoint")] = this->currentSP;
        (*packet)[F("TemperatureSetPoint")] = this->temperatureSP;
        (*packet)[F("RuntimeSeconds")] = this->runTimeSecs;
        (*packet)[F("ElapsedSeconds")] = this->elapsedSecs;
        (*packet)[F("Running")] = this->running;
        (*packet)[F("Paused")] = this->paused;
     }
};


