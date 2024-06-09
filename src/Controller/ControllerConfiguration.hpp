#pragma once
#include "../constants.h"
#include "../Serializable.hpp"
#include "../TestTimer/BurnTimerConfig.hpp"



struct ControllerConfig:public Serializable{
    BurnTimerConfig         burnTimerConfig;
    unsigned long           comInterval=COM_INTERVAL;
    unsigned long           updateInterval=UPDATE_INTERVAL;
    unsigned long           logInterval=LOG_INTERVAL;
    unsigned long           versionInterval=VER_CHECK_INTERVAL;
    float                   timeOffPercent=0.0f;

    void Reset(){
        burnTimerConfig.Reset();
        comInterval=COM_INTERVAL;
        updateInterval=UPDATE_INTERVAL;
        logInterval=LOG_INTERVAL;
        versionInterval=VER_CHECK_INTERVAL;
        timeOffPercent=0.0f;
    }

    virtual void Deserialize(JsonDocument &doc)override{
        JsonObject timeJson=doc[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];
        this->burnTimerConfig.timeOffPercent=timeJson[F("TimeOffPercent")];
        this->comInterval=doc[F("ComInterval")];
        this->updateInterval=doc[F("UpdateInterval")];
        this->logInterval=doc[F("LogInterval")];
        this->versionInterval=doc[F("VersionInterval")];
    }
    
    virtual void Deserialize(JsonObject &packet)override{
        JsonObject timeJson=packet[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];
        this->burnTimerConfig.timeOffPercent=timeJson[F("TimeOffPercent")];
        this->comInterval=packet[F("ComInterval")];
        this->updateInterval=packet[F("UpdateInterval")];
        this->logInterval=packet[F("LogInterval")];
        this->versionInterval=packet[F("VersionInterval")];

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
        timeJson[F("TimeOffPercent")]=this->burnTimerConfig.timeOffPercent;

        (*doc)[F("ComInterval")]=this->comInterval;
        (*doc)[F("UpdateInterval")]=this->updateInterval;
        (*doc)[F("LogInterval")]=this->logInterval;
        (*doc)[F("VersionInterval")]=this->versionInterval;
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
        timeJson[F("TimeOffPercent")]=this->burnTimerConfig.timeOffPercent;

        (*packet)[F("ComInterval")]=this->comInterval;
        (*packet)[F("UpdateInterval")]=this->updateInterval;
        (*packet)[F("LogInterval")]=this->logInterval;
        (*packet)[F("VersionInterval")]=this->versionInterval;
    }
};

