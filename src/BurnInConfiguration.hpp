#pragma once
#include <ArduinoJson.h>
#include "Serializable.hpp"
#include "Heaters/HeaterConfiguration.hpp"
#include "Probes/ProbeConfiguration.hpp"
#include "TestTimer/BurnTimerConfig.hpp"
#include "Controller/ControllerConfiguration.hpp"


/* class BurnInConfiguration:public Serializable{
public:
    HeaterControllerConfig heaterControllerConfig;
    ProbeControllerConfig probeControllerConfig;
    ControllerConfig controllerConfig;

    void Reset(){
        heaterControllerConfig.Reset();
        probeControllerConfig.Reset();
        controllerConfig.Reset();
    }
    virtual void Serialize(JsonDocument *doc,bool initialize) override{
        JsonObject heaterJson;
        JsonObject probeJson;
        JsonObject controllerJson;
        if(initialize){
            heaterJson=(*doc)[F("HeaterControllerConfig")].to<JsonObject>();
            probeJson=(*doc)[F("ProbeControllerConfig")].to<JsonObject>();
            controllerJson=(*doc)[F("ControllerConfig")].to<JsonObject>();
        }else{
            heaterJson=(*doc)[F("HeaterControllerConfig")].as<JsonObject>();
            probeJson=(*doc)[F("ProbeControllerConfig")].as<JsonObject>();
            controllerJson=(*doc)[F("ControllerConfig")].as<JsonObject>();
        }

        heaterControllerConfig.Serialize(&heaterJson,initialize);
        probeControllerConfig.Serialize(&probeJson,initialize);
        controllerConfig.Serialize(&controllerJson,initialize);
    }

    virtual void Serialize(JsonObject *packet,bool initialize){
        JsonObject heaterJson;
        JsonObject probeJson;
        JsonObject controllerJson;
        if(initialize){
            heaterJson=(*packet)[F("HeaterControllerConfig")].to<JsonObject>();
            probeJson=(*packet)[F("ProbeControllerConfig")].to<JsonObject>();
            controllerJson=(*packet)[F("ControllerConfig")].to<JsonObject>();
        }else{
            heaterJson=(*packet)[F("HeaterControllerConfig")].as<JsonObject>();
            probeJson=(*packet)[F("ProbeControllerConfig")].as<JsonObject>();
            controllerJson=(*packet)[F("ControllerConfig")].as<JsonObject>();
        }
        heaterControllerConfig.Serialize(&heaterJson,initialize);
        probeControllerConfig.Serialize(&probeJson,initialize);
        controllerConfig.Serialize(&controllerJson,initialize);
    }

    virtual void Deserialize(JsonDocument &doc) override{
        auto heaterConfig=doc[F("HeaterControllerConfig")].as<JsonObject>();
        auto probeConfig=doc[F("ProbeControllerConfig")].as<JsonObject>();
        auto controllerConfig=doc[F("ControllerConfig")].as<JsonObject>();
        this->heaterControllerConfig.Deserialize(heaterConfig);
        this->probeControllerConfig.Deserialize(probeConfig);
        this->controllerConfig.Deserialize(controllerConfig);
    }

    virtual void Deserialize(JsonObject &packet) override{
        auto heaterConfig=packet[F("HeaterControllerConfig")].as<JsonObject>();
        auto probeConfig=packet[F("ProbeControllerConfig")].as<JsonObject>();
        auto controllerConfig=packet[F("ControllerConfig")].as<JsonObject>();
        this->heaterControllerConfig.Deserialize(heaterConfig);
        this->probeControllerConfig.Deserialize(probeConfig);
        this->controllerConfig.Deserialize(controllerConfig);
    }
}; */
