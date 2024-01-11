#pragma once
#include <ArduinoJson.h>
#include "Serializable.hpp"
#include "../constants.h"


struct SerialDataOutput:public Serializable{
     double voltages[PROBE_COUNT]={0,0,0,0,0,0};
     double currents[PROBE_COUNT]={0,0,0,0,0,0};
     double temperatures[HEATER_COUNT]={0,0,0};
     double probeRuntimes[PROBE_COUNT]={0,0,0,0,0,0};
     bool heaterStates[HEATER_COUNT];
  
     bool running=false,paused=false;
     int currentSP=150;
     double temperatureSP=85;
     long runTimeSecs=54234;
     long elapsedSecs=12345;

     void Deserialize(const JsonDocument& doc) override{
        JsonArray v_array=doc["Voltages"].as<JsonArray>();
        JsonArray c_array=doc["Currents"].as<JsonArray>();
        JsonArray t_array=doc["Temperatures"].as<JsonArray>();
        JsonArray rt_array=doc["ProbeRuntimes"].as<JsonArray>();
        JsonArray h_array=doc["HeaterStates"].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            this->voltages[i]=v_array[i];
            this->currents[i]=c_array[i];
            this->probeRuntimes[i]=rt_array[i];
            if(i<HEATER_COUNT){
                this->temperatures[i]=t_array[i];
                this->heaterStates[i]=h_array[i];
            }
        }
        this->currentSP=doc["CurrentSetPoint"];
        this->temperatureSP=doc["TemperatureSetPoint"];
        this->runTimeSecs=doc["RuntimeSeconds"];
        this->elapsedSecs=doc["ElapsedSeconds"];
        this->running=doc["Running"];
        this->paused=doc["Paused"];
     }

    void Deserialize(const JsonDocument& packet) override{
        JsonArray v_array=packet["Voltages"].as<JsonArray>();
        JsonArray c_array=packet["Currents"].as<JsonArray>();
        JsonArray t_array=packet["Temperatures"].as<JsonArray>();
        JsonArray rt_array=packet["ProbeRuntimes"].as<JsonArray>();
        JsonArray h_array=packet["HeaterStates"].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            this->voltages[i]=v_array[i];
            this->currents[i]=c_array[i];
            this->probeRuntimes[i]=rt_array[i];
            if(i<HEATER_COUNT){
                this->temperatures[i]=t_array[i];
                this->heaterStates[i]=h_array[i];
            }
        }
        this->currentSP=packet["CurrentSetPoint"];
        this->temperatureSP=packet["TemperatureSetPoint"];
        this->runTimeSecs=packet["RuntimeSeconds"];
        this->elapsedSecs=packet["ElapsedSeconds"];
        this->running=packet["Running"];
        this->paused=packet["Paused"];
     }
     
     void Serialize(JsonDocument* doc,bool initialize) override{
        JsonArray v_array;
        JsonArray c_array;
        JsonArray t_array;
        JsonArray rt_array;
        JsonArray h_array;
        if(initialize){
            v_array=(*doc)["Voltages"].to<JsonArray>();
            c_array=(*doc)["Currents"].to<JsonArray>();
            t_array=(*doc)["Temperatures"].to<JsonArray>();
            rt_array=(*doc)["ProbeRuntimes"].to<JsonArray>();
            h_array=(*doc)["HeaterStates"].to<JsonArray>();
        }else{
            v_array=(*doc)["Voltages"].as<JsonArray>();
            c_array=(*doc)["Currents"].as<JsonArray>();
            t_array=(*doc)["Temperatures"].as<JsonArray>();
            rt_array=(*doc)["ProbeRuntimes"].as<JsonArray>();
            h_array=(*doc)["HeaterStates"].as<JsonArray>();
        }
        for(int i=0;i<PROBE_COUNT;i++){
            v_array[i]=this->voltages[i];
            c_array[i]=this->currents[i];
            rt_array[i]=this->probeRuntimes[i];
            if(i<HEATER_COUNT){
                t_array[i]=this->temperatures[i];
                h_array[i]=this->heaterStates[i];
            }
        }
        (*doc)["CurrentSetPoint"] = this->currentSP;
        (*doc)["TemperatureSetPoint"] = this->temperatureSP;
        (*doc)["RuntimeSeconds"] = this->runTimeSecs;
        (*doc)["ElapsedSeconds"] = this->elapsedSecs;
        (*doc)["Running"] = this->running;
        (*doc)["Paused"] = this->paused;
        //serializeJson(*doc, Serial);
     }

     void Serialize(JsonObject *packet,bool initialize) override{
        JsonArray v_array;
        JsonArray c_array;
        JsonArray t_array;
        JsonArray rt_array;
        JsonArray h_array;
        if(initialize){
            v_array=(*packet)["Voltages"].to<JsonArray>();
            c_array=(*packet)["Currents"].to<JsonArray>();
            t_array=(*packet)["Temperatures"].to<JsonArray>();
            rt_array=(*packet)["ProbeRuntimes"].to<JsonArray>();
            h_array=(*packet)["HeaterStates"].to<JsonArray>();
        }else{
            v_array=(*packet)["Voltages"].as<JsonArray>();
            c_array=(*packet)["Currents"].as<JsonArray>();
            t_array=(*packet)["Temperatures"].as<JsonArray>();
            rt_array=(*packet)["ProbeRuntimes"].as<JsonArray>();
            h_array=(*packet)["HeaterStates"].as<JsonArray>();
        }
        for(int i=0;i<PROBE_COUNT;i++){
            v_array[i]=this->voltages[i];
            c_array[i]=this->currents[i];
            rt_array[i]=this->probeRuntimes[i];
            if(i<HEATER_COUNT){
                t_array[i]=this->temperatures[i];
                h_array[i]=this->heaterStates[i];
            }
        }
        (*packet)["CurrentSetPoint"] = this->currentSP;
        (*packet)["TemperatureSetPoint"] = this->temperatureSP;
        (*packet)["RuntimeSeconds"] = this->runTimeSecs;
        (*packet)["ElapsedSeconds"] = this->elapsedSecs;
        (*packet)["Running"] = this->running;
        (*packet)["Paused"] = this->paused;
     }
};


class SerialComManager{
private: 
    StaticJsonDocument<384> outputDoc;
public:
    void CreateJsonDocuments(){

    }
};


