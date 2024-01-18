#pragma once
#include <ArduinoJson.h>
#include "../Serializable.hpp"
#include "probe_constants.h"

struct CurrentSelectorConfig{
    int pin120mA=PIN_CURRENT_120mA;
    int pin60mA=PIN_CURRENT_60mA;
    int currentPin=PIN_CURRENT;
    CurrentValue SetCurrent=DEFAULT_CURRENT;
    bool switchEnabled=true;
    
    void Deserialize(JsonObject& configJson){
        this->currentPin=configJson[F("CurrentPin")];
        this->pin120mA=configJson[F("Pin120mA")];
        this->pin60mA=configJson[F("Pin60mA")];
        this->SetCurrent=(CurrentValue)configJson[F("SetCurrent")];
        this->switchEnabled=configJson[F("SwitchEnabled")];
    }
    void Serialize(JsonObject* configJson){
        (*configJson)[F("CurrentPin")]=this->currentPin;
        (*configJson)[F("Pin120mA")]=this->pin120mA;
        (*configJson)[F("Pin60mA")]=this->pin60mA;
        (*configJson)[F("SetCurrent")]=this->SetCurrent;
        (*configJson)[F("SwitchEnabled")]=this->switchEnabled;   
    }
};


struct CurrentSensorConfig{
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

struct VoltageSensorConfig{
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

struct ProbeConfig{
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
    CurrentValue probeTestCurrent;
    double probeCurrentPercent;
    CurrentSelectorConfig   currentSelectConfig;
    
	ProbeConfig	probeConfigs[PROBE_COUNT]={
		ProbeConfig(VoltageSensorConfig(PIN_PROBE1_VOLT),CurrentSensorConfig(PIN_PROBE1_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE2_VOLT),CurrentSensorConfig(PIN_PROBE2_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE3_VOLT),CurrentSensorConfig(PIN_PROBE3_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE4_VOLT),CurrentSensorConfig(PIN_PROBE4_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE5_VOLT),CurrentSensorConfig(PIN_PROBE5_CURRENT)),
		ProbeConfig(VoltageSensorConfig(PIN_PROBE6_VOLT),CurrentSensorConfig(PIN_PROBE6_CURRENT))
	};

    virtual void Serialize(JsonDocument *doc,bool initialize) override{
        JsonObject selectJson;
        if(initialize){
            //to<JsonArray>() creates a new memory pointer
            selectJson=(*doc)[F("CurrentSelectConfig")].to<JsonObject>();
            JsonArray probeJsonConfigs = (*doc)[F("ProbeConfigurations")].to<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs.add<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,true);
            }
        }else{
            //as<JsonArray>() points to existing memory block
            selectJson=(*doc)[F("CurrentSelectConfig")].as<JsonObject>();
            JsonArray probeJsonConfigs = (*doc)[F("ProbeConfigurations")].as<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,false);
            }
        }
        this->currentSelectConfig.Serialize(&selectJson);
        (*doc)[F("ReadInterval")] = this->readInterval;
        (*doc)[F("CurrentPercent")]=this->probeCurrentPercent;
        (*doc)[F("ProbeTestCurrent")]=this->probeTestCurrent;
        //(*doc).shrinkToFit();
    }    
    
    virtual void Serialize(JsonObject *packet,bool initialize) override{
        JsonObject selectJson;
        if(initialize){
            //to creates a new memory pointer
            selectJson=(*packet)[F("CurrentSelectConfig")].to<JsonObject>();
            JsonArray probeJsonConfigs = (*packet)[F("ProbeConfigurations")].to<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs.add<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,true);
            }
        }else{
            //as points to existing memory block
            selectJson=(*packet)[F("CurrentSelectConfig")].as<JsonObject>();
            JsonArray probeJsonConfigs = (*packet)[F("ProbeConfigurations")].as<JsonArray>();
            for(int i=0;i<PROBE_COUNT;i++){
                auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
                this->probeConfigs[i].Serialize(&probeConfig,false);
            }
        }
        this->currentSelectConfig.Serialize(&selectJson);
        (*packet)[F("CurrentPercent")]=this->probeCurrentPercent;
        (*packet)[F("ProbeTestCurrent")]=this->probeTestCurrent;
        (*packet)[F("ReadInterval")] = this->readInterval;
    }

    virtual void Deserialize(JsonDocument &doc) override{
        JsonObject selectJson=doc[F("CurrentSelectConfig")].as<JsonObject>();
        this->currentSelectConfig.Deserialize(selectJson);
        auto probeJsonConfigs = doc[F("ProbeConfigurations")].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
            this->probeConfigs[i].Deserialize(probeConfig);
        }
        this->readInterval=doc[F("ReadInterval")];
        this->probeCurrentPercent=doc[F("CurrentPercent")];
        this->probeTestCurrent=doc[F("ProbeTestCurrent")];
    }

    virtual void Deserialize(JsonObject &packet) override{
        JsonObject selectJson=packet[F("CurrentSelectConfig")].as<JsonObject>();
        this->currentSelectConfig.Deserialize(selectJson);
        auto probeJsonConfigs = packet[F("ProbeConfigurations")].as<JsonArray>();
        for(int i=0;i<PROBE_COUNT;i++){
            auto probeConfig=probeJsonConfigs[i].as<JsonObject>();
            this->probeConfigs[i].Deserialize(probeConfig);
        }
        this->readInterval=packet[F("ReadInterval")];
        this->probeCurrentPercent=packet[F("CurrentPercent")];
        this->probeTestCurrent=packet[F("ProbeTestCurrent")];
    }
};