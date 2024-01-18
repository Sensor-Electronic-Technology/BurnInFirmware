#pragma once
#include <ArduinoJson.h>
#include "BurnInTimer.hpp"
#include "../Serializable.hpp"


struct SaveState:public Serializable{
    CurrentValue    setCurrent;
    int             setTemperature;
    TimerData       currentTimes;

    void Set(CurrentValue current,int temp,const TimerData& timeData){
        this->setCurrent=current;
        this->setTemperature=temp;
        this->currentTimes=timeData;
    }

    void Serialize(JsonDocument* doc,bool initialize){
        (*doc)[F("SetCurrent")]=this->setCurrent;
        (*doc)[F("SetTemperature")]=this->setTemperature;
        JsonObject jsonTime=(initialize) ? (*doc)[F("CurrentTime")].to<JsonObject>():(*doc)[F("CurrentTime")].as<JsonObject>();
        this->currentTimes.Serialize(&jsonTime,initialize);
    }

    void Deserialize(JsonDocument &doc){
        this->setCurrent=(CurrentValue)doc[F("SetCurrent")];
        this->setTemperature=doc[F("SetTemperature")];
        JsonObject jsonTime=doc[F("CurrentTime")].as<JsonObject>();
        this->currentTimes.Deserialize(jsonTime);
    }

    void Serialize(JsonObject* packet,bool initialize){
        (*packet)[F("SetCurrent")]=this->setCurrent;
        (*packet)[F("SetTemperature")]=this->setTemperature;
        JsonObject jsonTime=(initialize) ? (*packet)[F("CurrentTime")].to<JsonObject>():(*packet)[F("CurrentTime")].as<JsonObject>();
        this->currentTimes.Serialize(&jsonTime,initialize);
    }

    void Deserialize(JsonObject &packet){
        this->setCurrent=(CurrentValue)packet[F("SetCurrent")];
        this->setTemperature=packet[F("SetTemperature")];
        JsonObject jsonTime=packet[F("CurrentTime")].as<JsonObject>();
        this->currentTimes.Deserialize(jsonTime);
    }
};