#pragma once
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <ArduinoComponents.h>
#include <SD.h>
#include <SPI.h>
#include "../Communication/Serializable.hpp"
#include "../Heaters/HeaterConfiguration.hpp"
#include "../Probes/ProbeConfiguration.hpp"
#include "../constants.h"


using namespace components;

typedef Function<void(HeaterControllerConfig)> HeaterControlCallback;
typedef Function<void(ProbeControllerConfig)>  ProbeControlCallback;

class ConfigurationManager{
public:
    static ConfigurationManager* const Instance(){
        if(instance==nullptr){
            instance=new ConfigurationManager;
        }

        return instance;
    }
    
    static void LoadConfig(Serializable* config,PacketType configType){
        auto instance=ConfigurationManager::Instance();
        instance->InstanceLoadConfig(config,configType);
    }

    void InstanceLoadConfig(Serializable* config,PacketType configType){
        auto fileName=filenames[configType];
        this->file=SD.open(fileName);
        if(!this->file){
            Serial.print("Error Opening ");Serial.println(fileName);
            return;
        }
        this->doc.clear();
        auto error=deserializeJson(this->doc,this->file);
        if(error){
            Serial.println("Error: Failed to deserialize document");
            Serial.println(error.f_str());
            this->file.close();
            return;
        }
        serializeJsonPretty(instance->doc,Serial);
        config->Deserialize(instance->doc);
        this->file.close();
    }

    /**
     * @brief 
     * 
     * @param config Configuration of type ControllerConfiguration
     * @param fileNameIndex While file name
     */
    static void SaveConfig(Serializable* config,PacketType configType){
        auto instance=ConfigurationManager::Instance();
        instance->InstanceSaveConfig(config,configType);
    }

    void InstanceSaveConfig(Serializable* config,PacketType configType){
        auto fileName=filenames[configType];
        SD.remove(fileName);//overwrite file
        this->file=SD.open(fileName,FILE_WRITE);
        if(!this->file){
            Serial.print("Error Opening ");Serial.println(fileName);
            return;
        }
        this->doc.clear();
        config->Serialize(&this->doc,true);
        if(serializeJsonPretty(this->doc,this->fileWriteBuffer)==0){
            Serial.println("Error: Faild to write out configuration");
        }
        serializeJsonPretty(this->doc,Serial);
        this->fileWriteBuffer.flush();
        this->file.close();
    }

private:
    static ConfigurationManager* instance;
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};

    HeaterControllerConfig heaterConfig;
    ProbeControllerConfig  probeConfig;

    // HeaterControlCallback _heaterControlCallback=[](HeaterControllerConfig){};
    // ProbeControlCallback  _probeControlCallback=[](ProbeControllerConfig){};

    //SD   sd;
};