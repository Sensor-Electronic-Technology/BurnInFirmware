#pragma once
#include <ArduinoJson.h>
#include "../Communication/Serializable.hpp"
#include "../constants.h"


class CurrentSensorConfig{
public:
    CurrentSensorConfig(int pin=0,double fweight=DEFAULT_FWEIGHT)
	:Pin(pin),fWeight(fweight){}
	
    int Pin;
	double fWeight;

    void Deserialize(JsonObject configJson){
        this->Pin=configJson[F("Pin")];
        this->fWeight=configJson[F("fWeight")];
    }
    
    void Serialize(JsonObject *configJson){
        (*configJson)[F("Pin")] = this->Pin;
        (*configJson)[F("fWeight")] = this->fWeight;
    }

    void Serialize(JsonObject*,bool);

};

class VoltageSensorConfig{
public:
	int Pin;
	double fWeight;
	VoltageSensorConfig(int pin=0,double fweight=DEFAULT_FWEIGHT)
		:Pin(pin),fWeight(fweight){}

    void Deserialize(JsonObject &configJson){
        this->Pin=configJson[F("Pin")];
        this->fWeight=configJson[F("fWeight")];
    }
    
    void Serialize(JsonObject *configJson){
        (*configJson)[F("Pin")] = this->Pin;
        (*configJson)[F("fWeight")] = this->fWeight;
    }
    //virtual void Serialize(JsonObject*,bool);
};

class ProbeConfig{
public:
	VoltageSensorConfig voltageConfig;
	CurrentSensorConfig currentConfig;
	ProbeConfig(){}
	ProbeConfig(const VoltageSensorConfig& vConfig,const CurrentSensorConfig& cConfig,
                unsigned long readInterval=PROBE_READINTERVAL)
		:voltageConfig(vConfig),currentConfig(cConfig){
	}
    void Deserialize(JsonObject &probeJsonConfig) {
        JsonObject voltSenseConfig=probeJsonConfig[F("VoltageSensorConfig")].as<JsonObject>();
        JsonObject currentSenseConfig=probeJsonConfig[F("CurrentSensorConfig")].as<JsonObject>();
        this->voltageConfig.Deserialize(voltSenseConfig);
        this->currentConfig.Deserialize(currentSenseConfig);
    }

    void Serialize(JsonObject *probeConfig,bool initialize){
        JsonObject voltSenseConfig;
        JsonObject currentSenseConfig;
        if(initialize){
            voltSenseConfig=(*probeConfig)[F("VoltageSensorConfig")].to<JsonObject>();
            currentSenseConfig=(*probeConfig)[F("CurrentSensorConfig")].to<JsonObject>();

        }else{
            voltSenseConfig=(*probeConfig)[F("VoltageSensorConfig")].as<JsonObject>();
            currentSenseConfig=(*probeConfig)[F("CurrentSensorConfig")].as<JsonObject>();
        }
        this->voltageConfig.Serialize(&voltSenseConfig);
        this->currentConfig.Serialize(&currentSenseConfig);
    }
    //virtual void Serialize(JsonObject*);
};

class ProbeControllerConfig:public Serializable{
public:
	int readInterval=PROBE_READINTERVAL;
	ProbeConfig	probeConfigs[PROBE_COUNT]={
		ProbeConfig(VoltageSensorConfig(PIN_PROBE1_VOLT),CurrentSensorConfig(PIN_PROBE1_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE2_VOLT),CurrentSensorConfig(PIN_PROBE2_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE3_VOLT),CurrentSensorConfig(PIN_PROBE3_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE4_VOLT),CurrentSensorConfig(PIN_PROBE4_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE5_VOLT),CurrentSensorConfig(PIN_PROBE5_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE6_VOLT),CurrentSensorConfig(PIN_PROBE6_CURRENT))
	};

    virtual void Serialize(JsonDocument *doc,bool initialize) override{
        if(initialize){
            //to<JsonArray>() creates a new memory pointer
            JsonArray probeJsonConfigs = (*doc)[F("ProbeConfigurations")].to<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs.add<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,true);
            }
        }else{
            //as<JsonArray>() points to existing memory block
            JsonArray probeJsonConfigs = (*doc)[F("ProbeConfigurations")].as<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,false);
            }
        }
        (*doc)[F("ReadInterval")] = this->readInterval;
        (*doc).shrinkToFit();
    }    
    
    virtual void Serialize(JsonObject *packet,bool initialize) override{
        if(initialize){
            //to<JsonArray>() creates a new memory pointer
            JsonArray probeJsonConfigs = (*packet)[F("ProbeConfigurations")].to<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs.add<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,true);
            }
        }else{
            //as<JsonArray>() points to existing memory block
            JsonArray probeJsonConfigs = (*packet)[F("ProbeConfigurations")].as<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,false);
            }
        }
        (*packet)[F("ReadInterval")] = this->readInterval;
    }

    virtual void Deserialize(JsonDocument &doc) override{
        auto probeJsonConfigs = doc[F("ProbeConfigurations")].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
            this->probeConfigs[i].Deserialize(probeConfig);
        }
        this->readInterval=doc[F("ReadInterval")];
    }

    virtual void Deserialize(JsonObject &packet) override{
        auto probeJsonConfigs = packet[F("ProbeConfigurations")].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
            this->probeConfigs[i].Deserialize(probeConfig);
        }
        this->readInterval=packet[F("ReadInterval")];
    }
};