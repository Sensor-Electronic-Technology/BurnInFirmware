#pragma once
#include <ArduinoJson.h>
#include "../Serializable.hpp"
#include "heater_constants.h"



class AutoTuneResults:public Serializable{
public:
    HeaterTuneResult results[HEATER_COUNT];
    AutoTuneResults(){
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            this->results[i]=HeaterTuneResult();
        }
    }
    void clear(){
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            this->results[i].clear();
        }
    }

    void set(uint8_t index,const HeaterTuneResult& result){
        this->results[index]=result;
    }

    virtual void Serialize(JsonObject *packet,bool initialize){
        if(initialize){
            JsonArray turnResultsJson=(*packet)[F("AutoTuneResults")].to<JsonArray>();
            for(auto result:this->results){
                JsonObject resultJson=turnResultsJson.add<JsonObject>();
                resultJson[F("HeaterNumber")]=result.heaterNumber;
                resultJson[F("kp")]=result.kp;
                resultJson[F("ki")]=result.ki;
                resultJson[F("kd")]=result.kd;
            }
        }else{
            JsonArray turnResultsJson=(*packet)[F("AutoTuneResults")].as<JsonArray>();
            for(uint8_t i=0;i<HEATER_COUNT;i++){
                JsonObject resultJson=turnResultsJson[i].as<JsonObject>();
                resultJson[F("HeaterNumber")]=results[i].heaterNumber;
                resultJson[F("kp")]=results[i].kp;
                resultJson[F("ki")]=results[i].ki;
                resultJson[F("kd")]=results[i].kd;
            }
        }
    }
    virtual void Serialize(JsonDocument *doc,bool initialize)override{

    }
    virtual void Deserialize(JsonDocument &doc) override{

    }
    
    virtual void Deserialize(JsonObject &packet)override{

    }
};

class NtcConfig{
public:
	float aCoeff,bCoeff,cCoeff;
	uint8_t Pin;
	float fWeight;
	NtcConfig(uint8_t pin=0,float a=0,float b=0,float c=0,float fweight=DEFAULT_FWEIGHT)
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
	float kp,kd,ki;
	unsigned long windowSize;

    PidConfig(float _kp=KP_DEFAULT,float _ki=KI_DEFAULT,float _kd=KD_DEFAULT,unsigned long window=DEFAULT_WINDOW)
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
	float tempDeviation;
	uint8_t Pin;
	uint8_t HeaterId;
	NtcConfig ntcConfig;
    PidConfig pidConfig;
	HeaterConfig(
		const NtcConfig& ntc_config,
        const PidConfig& pid_config,
        uint8_t id=0,
        uint8_t pin=0,
		float tempDev=DEFAULT_TEMP_DEV)
			:HeaterId(id),Pin(pin),pidConfig(pidConfig),ntcConfig(ntc_config),
			tempDeviation(tempDev){	}

    void UpdatePid(HeaterTuneResult newPid){
        this->pidConfig.kp=newPid.kp;
        this->pidConfig.ki=newPid.ki;
        this->pidConfig.kd=newPid.kd;
    } 
    
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

/**
 * @brief Heater controller configuration
 * 
 */
class HeaterControllerConfig:public Serializable{
public:
	unsigned long readInterval=TEMP_INTERVAL;
	HeaterConfig heaterConfigs[HEATER_COUNT]={
		HeaterConfig(NtcConfig(PIN_HEATER1_TEMP,NTC1_A,NTC1_B,NTC1_C),PidConfig(242.21,1868.81,128.49,DEFAULT_WINDOW),1,PIN_HEATER1_HEATER),
		HeaterConfig(NtcConfig(PIN_HEATER2_TEMP,NTC2_A,NTC2_B,NTC2_C),PidConfig(765.77,1345.82,604.67,DEFAULT_WINDOW),2,PIN_HEATER2_HEATER),
		HeaterConfig(NtcConfig(PIN_HEATER3_TEMP,NTC3_A,NTC3_B,NTC3_C),PidConfig(179.95,2216.84,81.62,DEFAULT_WINDOW),3,PIN_HEATER3_HEATER)
	};

    void UpdateHeaterPid(HeaterTuneResult newPid){
        this->heaterConfigs[newPid.heaterNumber-1].UpdatePid(newPid);
    }

    virtual void Serialize(JsonDocument *doc,bool initialize) override{
        if(initialize){
            auto heaterJsonConfigs=(*doc)[F("HeaterConfigurations")].to<JsonArray>();
            for(uint8_t i=0;i<HEATER_COUNT;i++){
                auto heaterJsonConfig = heaterJsonConfigs.add<JsonObject>();
                this->heaterConfigs[i].Serialize(&heaterJsonConfig,true);
                
            }
        }else{
            auto heaterJsonConfigs=(*doc)[F("HeaterConfigurations")].as<JsonArray>();
            for(uint8_t i=0;i<HEATER_COUNT;i++){
                auto heaterJsonConfig = heaterJsonConfigs[i].as<JsonObject>();
                this->heaterConfigs[i].Serialize(&heaterJsonConfig,false);
            }
        }
        (*doc)[F("ReadInterval")] = this->readInterval;
    }

    virtual void Serialize(JsonObject *packet,bool initialize){
        if(initialize){
            auto heaterJsonConfigs=(*packet)[F("HeaterConfigurations")].to<JsonArray>();
            for(uint8_t i=0;i<HEATER_COUNT;i++){
                auto heaterJsonConfig = heaterJsonConfigs.add<JsonObject>();
                this->heaterConfigs[i].Serialize(&heaterJsonConfig,true);
                
            }
        }else{
            auto heaterJsonConfigs=(*packet)[F("HeaterConfigurations")].as<JsonArray>();
            for(uint8_t i=0;i<HEATER_COUNT;i++){
                auto heaterJsonConfig = heaterJsonConfigs[i].as<JsonObject>();
                this->heaterConfigs[i].Serialize(&heaterJsonConfig,false);
            }
        }
        (*packet)[F("ReadInterval")] = this->readInterval;
    }

    virtual void Deserialize(JsonDocument &doc) override{
        auto heaterJsonConfigs=doc[F("HeaterConfigurations")].as<JsonArray>();
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            JsonObject heaterJson=heaterJsonConfigs[i].as<JsonObject>();
            this->heaterConfigs[i].Deserialize(heaterJson);   
        }
        this->readInterval = doc[F("ReadInterval")];
    }

    virtual void Deserialize(JsonObject &packet) override{
        auto heaterJsonConfigs=packet[F("HeaterConfigurations")].as<JsonArray>();
        for(uint8_t i=0;i<HEATER_COUNT;i++){
            JsonObject heaterJson=heaterJsonConfigs[i].as<JsonObject>();
            this->heaterConfigs[i].Deserialize(heaterJson);   
        }
        this->readInterval = packet[F("ReadInterval")];
    }
};