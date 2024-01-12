#pragma once
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <ArduinoComponents.h>
#include <SD.h>
#include <SPI.h>
#include "../Serializable.hpp"
#include "../Heaters/HeaterConfiguration.hpp"
#include "../Probes/ProbeConfiguration.hpp"
#include "../Logging/StationLogger.hpp"
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
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                "Configuration file failed to open.  System will switch to default setup \n Please contact administrator asap!",
                read_packet_prefix(configType));
            return;
        }
        this->doc.clear();
        auto error=deserializeJson(this->doc,this->file);
        if(error){
            Serial.println("Error: Failed to deserialize document");
            Serial.println(error.f_str());
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                "Failed to load configuration %s.  System will switch to default values. \n Please contact administrator asap!",
                read_packet_prefix(configType));
            this->file.close();
            return;
        }
        //serializeJsonPretty(instance->doc,Serial);
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
            //Serial.print("Error Opening ");Serial.println(fileName);
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                    "Heater Configuration file failed to open.  Please save values and restart the system");
            return;
        }
        this->doc.clear();
        config->Serialize(&this->doc,true);
        if(serializeJsonPretty(this->doc,this->fileWriteBuffer)==0){
            //Serial.println("Error: Faild to write out configuration");
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                    "Heater Configuration save failed. Changed will be lost on restart.\n Suggest manually enter changes");
        }
        //TODO: test this
        ComHandler::MsgPacketSerializer(*config,configType);
        //serializeJsonPretty(this->doc,Serial);
        this->fileWriteBuffer.flush();
        this->file.close();
    }

private:
    static ConfigurationManager* instance;
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};

    // HeaterControllerConfig heaterConfig;
    // ProbeControllerConfig  probeConfig;
};