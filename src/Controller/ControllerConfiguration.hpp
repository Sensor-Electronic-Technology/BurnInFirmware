#pragma once
#include "../constants.h"
#include "../Serializable.hpp"

struct BurnTimerConfig{
	unsigned long time60mASecs=TIME_SECS_60mA;
	unsigned long time120mASecs=TIME_SECS_120mA;
	unsigned long time150mASecs=TIME_SECS_150mA;
};

struct ControllerConfig:public Serializable{
    BurnTimerConfig         burnTimerConfig;
    unsigned long           comInterval;
    unsigned long           updateInterval;
    unsigned long           logInterval;

    virtual void Deserialize(JsonDocument &doc)override{
        JsonObject timeJson=doc[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];
    }
    
    virtual void Deserialize(JsonObject &packet)override{
        JsonObject timeJson=packet[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];

    }
    virtual void Serialize(JsonDocument *doc,bool initialize)override{
        JsonObject timeJson;
        if(initialize){       
           timeJson=(*doc)[F("BurnTimerConfig")].to<JsonObject>();
        }else{   
            timeJson=(*doc)[F("BurnTimerConfig")].as<JsonObject>();
        }

        timeJson[F("Time60mASec")]=this->burnTimerConfig.time60mASecs;
        timeJson[F("Time120mASec")]=this->burnTimerConfig.time120mASecs;
        timeJson[F("Time150mASec")]=this->burnTimerConfig.time150mASecs;
    }

    virtual void Serialize(JsonObject *packet,bool initialize)override{
        JsonObject timeJson;
        if(initialize){
           timeJson=(*packet)[F("BurnTimerConfig")].to<JsonObject>();
        }else{
            timeJson=(*packet)[F("BurnTimerConfig")].as<JsonObject>();
        }
        timeJson[F("Time60mASec")]=this->burnTimerConfig.time60mASecs;
        timeJson[F("Time120mASec")]=this->burnTimerConfig.time120mASecs;
        timeJson[F("Time150mASec")]=this->burnTimerConfig.time150mASecs;
    }
};

