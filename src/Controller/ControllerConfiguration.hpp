#pragma once
#include "../constants.h"
#include "../Serializable.hpp"

struct CurrentSelectorConfig{
    int pin120mA=PIN_CURRENT_120mA;
    int pin60mA=PIN_CURRENT_60mA;
    int currentPin=PIN_CURRENT;
    CurrentValue SetCurrent=DEFAULT_CURRENT;
    bool switchEnabled=true;
};

struct BurnTimerConfig{
	unsigned long time60mASecs=TIME_SECS_60mA;
	unsigned long time120mASecs=TIME_SECS_120mA;
	unsigned long time150mASecs=TIME_SECS_150mA;
};


struct ControllerConfig:public Serializable{
    CurrentSelectorConfig currentSelectConfig;
    BurnTimerConfig   burnTimerConfig;
    virtual void Deserialize(JsonDocument &doc)override{
        JsonObject selectJson=doc[F("CurrentSelectConfig")].as<JsonObject>();
        this->currentSelectConfig.currentPin=selectJson[F("CurrentPin")];
        this->currentSelectConfig.pin120mA=selectJson[F("Pin120mA")];
        this->currentSelectConfig.pin60mA=selectJson[F("Pin60mA")];
        this->currentSelectConfig.SetCurrent=(CurrentValue)selectJson[F("SetCurrent")].as<int>();
        this->currentSelectConfig.switchEnabled=selectJson[F("SwitchEnabled")];
        JsonObject timeJson=doc[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];
    }
    virtual void Deserialize(JsonObject &packet)override{
        JsonObject selectJson=packet[F("CurrentSelectConfig")].as<JsonObject>();
        this->currentSelectConfig.currentPin=selectJson[F("CurrentPin")];
        this->currentSelectConfig.pin120mA=selectJson[F("Pin120mA")];
        this->currentSelectConfig.pin60mA=selectJson[F("Pin60mA")];
        this->currentSelectConfig.SetCurrent=(CurrentValue)selectJson[F("SetCurrent")].as<int>();
        this->currentSelectConfig.switchEnabled=selectJson[F("SwitchEnabled")];
        JsonObject timeJson=packet[F("BurnTimerConfig")].as<JsonObject>();
        this->burnTimerConfig.time60mASecs=timeJson[F("Time60mASec")];
        this->burnTimerConfig.time120mASecs=timeJson[F("Time120mASec")];
        this->burnTimerConfig.time150mASecs=timeJson[F("Time150mASec")];

    }
    virtual void Serialize(JsonDocument *doc,bool initialize)override{
        JsonObject selectJson;
        JsonObject timeJson;
        if(initialize){
           selectJson=(*doc)[F("CurrentSelectConfig")].to<JsonObject>();
           timeJson=(*doc)[F("BurnTimerConfig")].to<JsonObject>();
        }else{
            selectJson=(*doc)[F("CurrentSelectConfig")].as<JsonObject>();
            timeJson=(*doc)[F("BurnTimerConfig")].as<JsonObject>();
        }

        selectJson[F("CurrentPin")]=this->currentSelectConfig.currentPin;
        selectJson[F("Pin120mA")]=this->currentSelectConfig.pin120mA;
        selectJson[F("Pin60mA")]=this->currentSelectConfig.pin60mA;
        selectJson[F("SetCurrent")]=this->currentSelectConfig.SetCurrent;
        selectJson[F("SwitchEnabled")]=this->currentSelectConfig.switchEnabled;
        timeJson[F("Time60mASec")]=this->burnTimerConfig.time60mASecs;
        timeJson[F("Time120mASec")]=this->burnTimerConfig.time120mASecs;
        timeJson[F("Time150mASec")]=this->burnTimerConfig.time150mASecs;
    }

    virtual void Serialize(JsonObject *packet,bool initialize)override{
        JsonObject selectJson;
        JsonObject timeJson;
        if(initialize){
           selectJson=(*packet)[F("CurrentSelectConfig")].to<JsonObject>();
           timeJson=(*packet)[F("BurnTimerConfig")].to<JsonObject>();
        }else{
            selectJson=(*packet)[F("CurrentSelectConfig")].as<JsonObject>();
            timeJson=(*packet)[F("BurnTimerConfig")].as<JsonObject>();
        }

        selectJson[F("CurrentPin")]=this->currentSelectConfig.currentPin;
        selectJson[F("Pin120mA")]=this->currentSelectConfig.pin120mA;
        selectJson[F("Pin60mA")]=this->currentSelectConfig.pin60mA;
        selectJson[F("SetCurrent")]=this->currentSelectConfig.SetCurrent;
        selectJson[F("SwitchEnabled")]=this->currentSelectConfig.switchEnabled;
        timeJson[F("Time60mASec")]=this->burnTimerConfig.time60mASecs;
        timeJson[F("Time120mASec")]=this->burnTimerConfig.time120mASecs;
        timeJson[F("Time150mASec")]=this->burnTimerConfig.time150mASecs;
    }
};

