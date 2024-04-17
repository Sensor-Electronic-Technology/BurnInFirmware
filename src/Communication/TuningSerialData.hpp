#pragma once
#include <ArduinoJson.h>
#include "../Heaters/heater_constants.h"
#include "../Serializable.hpp"
#include "../constants.h"

class TuningSerialData:public Serializable{
public:
     float temperatures[HEATER_COUNT]={0,0,0};
     bool heaterStates[HEATER_COUNT]={false,false,false};
     float temperatureSp=85;
     unsigned long elapsedSecs=0;
     bool isTuning=false;

    void Set(HeaterResult heaterResults[HEATER_COUNT],float tempSp,unsigned long elapsed,bool tuning){ 
        
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            auto result=heaterResults[i];
            this->heaterStates[i]=result.state;
            this->temperatures[i]=result.temperature;
        }
        this->temperatureSp=tempSp;
        this->isTuning=tuning;
        this->elapsedSecs=elapsed;
     }

     void Deserialize(JsonDocument& doc) override{
        JsonArray t_array=doc[F("Temperatures")].as<JsonArray>();
        JsonArray h_array=doc[F("HeaterStates")].as<JsonArray>();
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            this->temperatures[i]=t_array[i];
            this->heaterStates[i]=h_array[i];
        }
        this->temperatureSp=doc[F("TemperatureSetPoint")];
        this->elapsedSecs=doc[F("ElapsedSeconds")];
        this->isTuning=doc[F("IsTuning")];
     }

    void Deserialize(JsonObject& packet) override{
        JsonArray t_array=packet[F("Temperatures")].as<JsonArray>();
        JsonArray h_array=packet[F("HeaterStates")].as<JsonArray>();
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            this->temperatures[i]=t_array[i];
            this->heaterStates[i]=h_array[i];
        }
        this->temperatureSp=packet[F("TemperatureSetPoint")];
        this->elapsedSecs=packet[F("ElapsedSeconds")];
        this->isTuning=packet[F("IsTuning")];
     }
     
     void Serialize(JsonDocument* doc,bool initialize) override{
        JsonArray t_array;
        JsonArray h_array;
        if(initialize){
            t_array=(*doc)[F("Temperatures")].to<JsonArray>();
            h_array=(*doc)[F("HeaterStates")].to<JsonArray>();
        }else{
            t_array=(*doc)[F("Temperatures")].as<JsonArray>();
            h_array=(*doc)[F("HeaterStates")].as<JsonArray>();
        }
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            t_array[i]=this->temperatures[i];
            h_array[i]=this->heaterStates[i];

        }
        (*doc)[F("TemperatureSetPoint")] = this->temperatureSp;
        (*doc)[F("ElapsedSeconds")] = this->elapsedSecs;
        (*doc)[F("IsTuning")] = this->isTuning;
     }

     void Serialize(JsonObject *packet,bool initialize) override{
        JsonArray t_array;
        JsonArray h_array;
        if(initialize){
            t_array=(*packet)[F("Temperatures")].to<JsonArray>();
            h_array=(*packet)[F("HeaterStates")].to<JsonArray>();
        }else{
            t_array=(*packet)[F("Temperatures")].as<JsonArray>();
            h_array=(*packet)[F("HeaterStates")].as<JsonArray>();
        }
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            t_array[i]=this->temperatures[i];
            h_array[i]=this->heaterStates[i];

        }
        (*packet)[F("TemperatureSetPoint")] = this->temperatureSp;
        (*packet)[F("ElapsedSeconds")] = this->elapsedSecs;
        (*packet)[F("IsTuning")] = this->isTuning;
     }
};