#pragma once
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <ArduinoComponents.h>
/* #include <SD.h>
#include <SPI.h> */
#include <SdFat.h>
#include "../Serializable.hpp"
#include "../Heaters/HeaterConfiguration.hpp"
#include "../Probes/ProbeConfiguration.hpp"
#include "../constants.h"
#include "../Communication/ComHandler.hpp"


using namespace components;

typedef Function<void(HeaterControllerConfig)> HeaterControlCallback;
typedef Function<void(ProbeControllerConfig)>  ProbeControlCallback;



class FileManager{
public:
    static FileManager* const Instance(){
        if(instance==nullptr){
            instance=new FileManager;
        }
        return instance;
    }

    static void Init(){
        auto instance=FileManager::Instance();
        if(!instance->sd.begin(SD_CONFIG)){
            sdInitialized=false;
            return;
        }
        sdInitialized=true;
    }

    static void FormatCard(){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            instance->InstanceFormatCard();
            return;
        }
        ComHandler::SendErrorMessage(SystemError::SD_FORMAT_FAILED,0,0);
    }

    static void FormatNoBackup(){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            instance->sd.format(&Serial);
            return;
        }
        ComHandler::SendErrorMessage(SystemError::SD_FORMAT_FAILED,0,0);
    }
    
    static bool LoadConfiguration(Serializable* config,ConfigType configType){
        auto instance=FileManager::Instance();
         if(sdInitialized){
            digitalWrite(LED_BUILTIN,HIGH);
            instance->InstanceLoadConfig(config,configType);
            digitalWrite(LED_BUILTIN,LOW);
        }
    }

    static FileResult LoadState(Serializable* sysState){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            digitalWrite(LED_BUILTIN,HIGH);
            auto fileResult=instance->InstanceLoadState(sysState);
            digitalWrite(LED_BUILTIN,LOW);
            return fileResult;
        }
        return FileResult::SD_NOT_INITIALIZED;
    }

    static bool ClearState(){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            digitalWrite(LED_BUILTIN,HIGH);
            auto fileResult= instance->InstanceClearState();
            digitalWrite(LED_BUILTIN,LOW);
            return fileResult;
        }
        return false;
    }
    /**
     * @brief 
     * 
     * @param config Configuration of type ControllerConfiguration
     * @param fileNameIndex While file name
     */
    static bool SaveConfiguration(Serializable* config,ConfigType configType){
        auto instance=FileManager::Instance();
        bool result=false;
        if(sdInitialized){
            digitalWrite(LED_BUILTIN,HIGH);
            result=instance->InstanceSaveConfig(config,configType);
            digitalWrite(LED_BUILTIN,LOW);
        }
        return result;
    }

    static bool SaveState(Serializable* sysState){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            digitalWrite(LED_BUILTIN,HIGH);
            auto fileResult=instance->InstanceSaveState(sysState);
            digitalWrite(LED_BUILTIN,LOW);
            return fileResult;
        }
        return false;
    }
private:
    bool InstanceLoadConfig(Serializable* config,ConfigType configType);
    bool InstanceSaveState(Serializable* config);
    bool InstanceSaveConfig(Serializable* config,ConfigType configType);
    bool InstanceClearState();
    bool InstanceFormatCard();
    FileResult InstanceLoadState(Serializable* config);
private:
    static FileManager* instance;
    SdFs sd;
};