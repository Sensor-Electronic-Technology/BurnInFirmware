#pragma once
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <SD.h>
#include <SPI.h>
#include "Configuration.hpp"
#include "../constants.h"


class ConfigurationManager{
public:
    static ConfigurationManager* const Instance(){
        if(instance==nullptr){
            instance=new ConfigurationManager;
        }

        return instance;
    }


    static  void LoadConfig(ControllerConfiguration& config,ConfigType configType){
        auto instance=ConfigurationManager::Instance();
        auto fileName=json_filenames[configType];
        instance->file=SD.open(fileName);
        if(!instance->file){
            Serial.print("Error Opening ");Serial.println(fileName);
            return;
        }
        instance->doc.clear();
        auto error=deserializeJson(instance->doc,instance->file);
        if(error){
            Serial.println("Error: Failed to deserialize document");
            Serial.println(error.f_str());
            instance->file.close();
            return;
        }
        serializeJsonPretty(instance->doc,Serial);
        config.Deserialize(instance->doc);
        instance->file.close();
    }

    /**
     * @brief 
     * 
     * @param config Configuration of type ControllerConfiguration
     * @param fileNameIndex While file name
     */
    static void SaveConfig(ControllerConfiguration& config,ConfigType configType){
        auto instance=ConfigurationManager::Instance();
        auto fileName=json_filenames[configType];
        SD.remove(fileName);//overwrite file
        instance->file=SD.open(fileName,FILE_WRITE);
        if(!instance->file){
            Serial.print("Error Opening ");Serial.println(fileName);
            return;
        }
        instance->doc.clear();
        config.Serialize(&instance->doc,true);
        if(serializeJsonPretty(instance->doc,instance->fileWriteBuffer)==0){
            Serial.println("Error: Faild to write out configuration");
        }
        serializeJsonPretty(instance->doc,Serial);
        instance->fileWriteBuffer.flush();
        instance->file.close();
    }

private:
    static ConfigurationManager* instance;
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    //SD   sd;
};