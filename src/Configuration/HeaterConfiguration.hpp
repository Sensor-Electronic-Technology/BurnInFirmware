#pragma once
#include <ArduinoJson.h>
#include "Configuration.hpp"
#include "../constants.h"

class NtcConfig{
public:
	double aCoeff,bCoeff,cCoeff;
	int Pin;
	double fWeight;
	NtcConfig(int pin=0,double a=0,double b=0,double c=0,double fweight=DEFAULT_FWEIGHT)
			:Pin(pin),aCoeff(a),bCoeff(b),cCoeff(c),fWeight(fweight){
	}
    void Deserialize(JsonObject config){
        this->aCoeff = config[F("ACoeff")];
        this->bCoeff = config[F("BCoeff")];
        this->cCoeff = config[F("CCoeff")];
        this->Pin= config[F("Pin")];
        this->fWeight = config[F("fWeight")]; 
    }

    void Serialize(JsonObject* config){
        (*config)[F("ACoeff")]=this->aCoeff;
        (*config)[F("BCoeff")]=this->bCoeff;
        (*config)[F("CCoeff")]=this->cCoeff;
        (*config)[F("Pin")]=this->Pin;
        (*config)[F("fWeight")]=this->fWeight;
    }

    //virtual void Serialize(JsonObject*,bool);
};

class PidConfig{
public:
	double kp,kd,ki;
	unsigned long windowSize;

    PidConfig(double _kp=KP_DEFAULT,double _ki=KI_DEFAULT,double _kd=KD_DEFAULT,unsigned long window=DEFAULT_WINDOW)
    :kp(_kp),ki(_ki),kd(_kd),windowSize(window){  }

    void Serialize(JsonObject *pidJson){
        (*pidJson)[F("Kp")] = this->kp;
        (*pidJson)[F("Ki")] = this->ki;
        (*pidJson)[F("Kd")] = this->kd;
        (*pidJson)[F("WindowSizeMs")] = this->windowSize;
    }

    void Deserialize(JsonObject pidJson){
        this->kp=pidJson[F("Kp")];
        this->ki=pidJson[F("Ki")];
        this->kd=pidJson[F("Kd")];
        this->windowSize=pidJson[F("WindowSizeMs")];
    }

    //virtual void Serialize(JsonObject*,bool);
};

class HeaterConfig{
public:
	double tempDeviation;
	int Pin;
	int HeaterId;
	NtcConfig ntcConfig;
    PidConfig pidConfig;
	HeaterConfig(
		const NtcConfig& ntc_config,
        const PidConfig& pid_config,
        int id=0,
        int pin=0,
		double tempDev=DEFAULT_TEMP_DEV)
			:HeaterId(id),Pin(pin),pidConfig(pidConfig),ntcConfig(ntc_config),
			tempDeviation(tempDev){	}
    void Deserialize(JsonObject heaterJson){
        JsonObject ntcConfigJson=heaterJson[F("NtcConfig")];
        JsonObject pidConfigJson=heaterJson[F("PidConfig")];
        this->ntcConfig.Deserialize(ntcConfigJson);
        this->pidConfig.Deserialize(pidConfigJson);
        this->tempDeviation = heaterJson[F("TempDeviation")];
        this->Pin = heaterJson[F("Pin")]; 
        this->HeaterId = heaterJson[F("HeaterId")]; 
    }

    void Serialize(JsonObject *heaterJson,bool initialize){
        JsonObject ntcJson;
        JsonObject pidJson;
        if(initialize){
            ntcJson=(*heaterJson)[F("NtcConfig")].to<JsonObject>();
            pidJson=(*heaterJson)[F("PidConfig")].to<JsonObject>();

        }else{
            ntcJson=(*heaterJson)[F("NtcConfig")].as<JsonObject>();
            pidJson=(*heaterJson)[F("PidConfig")].as<JsonObject>();
        }
        this->ntcConfig.Serialize(&ntcJson);
        this->pidConfig.Serialize(&pidJson);
        (*heaterJson)[F("TempDeviation")] = this->tempDeviation;
        (*heaterJson)[F("Pin")] = this->Pin;
        (*heaterJson)[F("HeaterId")] = this->HeaterId;
    }
    //virtual void Serialize(JsonObject*);
};

class HeaterControllerConfig:public ControllerConfiguration{
public:
	unsigned long readInterval=TEMP_INTERVAL;
	HeaterConfig heaterConfigs[HEATER_COUNT]={
		HeaterConfig(NtcConfig(PIN_HEATER1_TEMP,NTC1_A,NTC1_B,NTC1_C),PidConfig(242.21,1868.81,128.49,DEFAULT_WINDOW),1,PIN_HEATER1_HEATER),
		HeaterConfig(NtcConfig(PIN_HEATER2_TEMP,NTC2_A,NTC2_B,NTC2_C),PidConfig(765.77,1345.82,604.67,DEFAULT_WINDOW),2,PIN_HEATER2_HEATER),
		HeaterConfig(NtcConfig(PIN_HEATER3_TEMP,NTC3_A,NTC3_B,NTC3_C),PidConfig(179.95,2216.84,81.62,DEFAULT_WINDOW),3,PIN_HEATER3_HEATER)
	};

    virtual void Deserialize(JsonDocument doc) override{
        auto heaterJsonConfigs=doc["HeaterConfigurations"].as<JsonArray>();
        for(int i=0;i<HEATER_COUNT;i++){
            JsonObject heaterJson=heaterJsonConfigs[i].as<JsonObject>();
            this->heaterConfigs[i].Deserialize(heaterJson);   
        }
        this->readInterval = doc["ReadInterval"];
    }

    virtual void Serialize(JsonDocument *doc,bool initialize) override{
        if(initialize){
            auto heaterJsonConfigs=(*doc)[F("HeaterConfigurations")].to<JsonArray>();
            for(int i=0;i<HEATER_COUNT;i++){
                auto heaterJsonConfig = heaterJsonConfigs.add<JsonObject>();
                this->heaterConfigs[i].Serialize(&heaterJsonConfig,true);
                
            }
        }else{
            auto heaterJsonConfigs=(*doc)[F("HeaterConfigurations")].as<JsonArray>();
            for(int i=0;i<HEATER_COUNT;i++){
                auto heaterJsonConfig = heaterJsonConfigs[i].as<JsonObject>();
                this->heaterConfigs[i].Serialize(&heaterJsonConfig,false);
            }
        }
        (*doc)["ReadInterval"] = this->readInterval;
    }
};