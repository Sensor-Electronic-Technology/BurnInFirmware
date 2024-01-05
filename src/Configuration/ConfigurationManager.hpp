#pragma once
#include "HeaterConfiguration.hpp"
#include "ProbeConfiguration.hpp"
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <SD.h>
#include <SPI.h>
#include "../constants.h"


class ConfigurationManager{
public:
    static ConfigurationManager* const Instance(){
        if(instance==nullptr){
            instance=new ConfigurationManager;
        }

        return instance;
    }


    static  void LoadConfig(ControllerConfiguration& config,int fileNameIndex){
        auto instance=ConfigurationManager::Instance();
        auto fileName=json_filenames[fileNameIndex];
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

    static void SaveConfig(ControllerConfiguration& config,int fileNameIndex){
        auto instance=ConfigurationManager::Instance();
        auto fileName=json_filenames[fileNameIndex];
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

  /*  static void LoadHeaterControllerConfig(HeaterControllerConfig& heaterCntlConfig){
        auto instance=ConfigurationManager::Instance();
        auto fileName=json_filenames[HEATER_CONFIG_INDEX];
        Serial.print("Opening File: ");Serial.println(fileName);
        File file=SD.open(fileName);
        if(!file){
            Serial.println("Error Opening HeaterConfigs.txt");
            return;
        }
        instance->heaterDoc.clear();
        auto error=deserializeJson(instance->heaterDoc,file);
        if(error){
            Serial.println("Error: Failed to deserialize document");
            Serial.println(error.f_str());
            file.close();
            return;
        }
        serializeJsonPretty(instance->heaterDoc,Serial);
        heaterCntlConfig.Deserialize(instance->heaterDoc);
        Serial.print("Heater 1 Kp: "); Serial.println(heaterCntlConfig.heaterConfigs[0].pidConfig.kp);
        file.close();
    }

    static void SaveHeaterControllerConfig(HeaterControllerConfig& heaterCntlConfig){
        auto instance=ConfigurationManager::Instance();
        auto fileName=json_filenames[HEATER_CONFIG_INDEX];
        SD.remove(fileName);//overwrite file
        instance->file=SD.open(fileName,FILE_WRITE);
        WriteBufferingStream fileWriteBuffer{instance->file, 64};
        if(!instance->file){
            Serial.print("Error Opening ");Serial.println(fileName);
            return;
        }
        instance->heaterDoc.clear();
        heaterCntlConfig.Serialize(&instance->heaterDoc,true);
        if(serializeJsonPretty(instance->heaterDoc,fileWriteBuffer)==0){
            Serial.println("Error: Faild to write out configuration");
        }
        serializeJsonPretty(instance->heaterDoc,Serial);
        fileWriteBuffer.flush();
        instance->file.close();
    }
*/

private:
    static ConfigurationManager* instance;
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    //SD   sd;
};