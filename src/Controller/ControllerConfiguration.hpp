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
    unsigned long           comInterval=COM_INTERVAL;
    unsigned long           updateInterval=UPDATE_INTERVAL;
    unsigned long           logInterval=LOG_INTERVAL;

    virtual void Deserialize(JsonDocument &doc)override{
        JsonObject timeJson=doc[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];
        this->comInterval=doc[F("ComInterval")];
        this->updateInterval=doc[F("UpdateInterval")];
        this->logInterval=doc[F("LogInterval")];
    }
    
    virtual void Deserialize(JsonObject &packet)override{
        JsonObject timeJson=packet[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];
        this->comInterval=packet[F("ComInterval")];
        this->updateInterval=packet[F("UpdateInterval")];
        this->logInterval=packet[F("LogInterval")];

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

        (*doc)[F("ComInterval")]=this->comInterval;
        (*doc)[F("UpdateInterval")]=this->updateInterval;
        (*doc)[F("LogInterval")]=this->logInterval;
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

        (*packet)[F("ComInterval")]=this->comInterval;
        (*packet)[F("UpdateInterval")]=this->updateInterval;
        (*packet)[F("LogInterval")]=this->logInterval;
    }
};

