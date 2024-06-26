#pragma once
#include <ArduinoJson.h>
#include "../TestTimer/BurnInTimer.hpp"
#include "../Serializable.hpp"


class SaveState:public Serializable{
public:
    CurrentValue    setCurrent;
    uint8_t         setTemperature;
    TimerData       currentTimes;
    String          testId;

    SaveState(){
        this->setCurrent=CurrentValue::c060;
        this->setTemperature=0;
        this->testId.reserve(TEST_ID_LENGTH);
        for(int i=0;i<TEST_ID_LENGTH;i++){
            this->testId[i]='x';
        }
    }

    void Clear(){
        this->setCurrent=CurrentValue::c060;
        this->setTemperature=0;
        this->currentTimes.Reset();
        for(int i=0;i<TEST_ID_LENGTH;i++){
            this->testId[i]='x';
        }
    }

    void Set(CurrentValue current,uint8_t temp,const TimerData& timeData,const char* testId){
        this->setCurrent=current;
        this->setTemperature=temp;
        this->currentTimes=timeData;
        this->testId=testId;
    }

    void Serialize(JsonDocument *doc,bool initialize) override{
        (*doc)[F("TestId")]=this->testId.c_str();
        (*doc)[F("SetCurrent")]=this->setCurrent;     
        (*doc)[F("SetTemperature")]=this->setTemperature;
        JsonObject jsonTime;
        if(initialize){
            jsonTime=(*doc)[F("CurrentTime")].to<JsonObject>();
        }else{
            jsonTime=(*doc)[F("CurrentTime")].as<JsonObject>();
        }
        this->currentTimes.Serialize(&jsonTime,initialize);
    }
    
    void Serialize(JsonObject *packet,bool initialize) override{
        (*packet)[F("TestId")]=this->testId.c_str();
        (*packet)[F("SetCurrent")]=this->setCurrent;
        (*packet)[F("SetTemperature")]=this->setTemperature;
        JsonObject jsonTime;
        if(initialize){
            jsonTime=(*packet)[F("CurrentTime")].to<JsonObject>();
        }else{
            jsonTime=(*packet)[F("CurrentTime")].as<JsonObject>();
        }
        this->currentTimes.Serialize(&jsonTime,initialize);
    }

    void Deserialize(JsonDocument &doc) override{
        this->testId=doc[F("TestId")].as<String>();
        this->setCurrent=(CurrentValue)doc[F("SetCurrent")];
        this->setTemperature=doc[F("SetTemperature")];
        JsonObject jsonTime=doc[F("CurrentTime")].as<JsonObject>();
        this->currentTimes.Deserialize(jsonTime);
    }

    void Deserialize(JsonObject &packet) override{
        this->testId=packet[F("TestId")].as<String>();
        this->setCurrent=(CurrentValue)packet[F("SetCurrent")];
        this->setTemperature=packet[F("SetTemperature")];
        JsonObject jsonTime=packet[F("CurrentTime")].as<JsonObject>();
        this->currentTimes.Deserialize(jsonTime);
    }
};