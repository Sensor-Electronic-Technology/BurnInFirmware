#pragma once
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <ArduinoComponents.h>
#include <SD.h>
#include <SPI.h>
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
    
    static void Load(Serializable* config,PacketType configType){
        auto instance=FileManager::Instance();
         if(sdInitialized){
            instance->InstanceLoadConfig(config,configType);
        }
    }

    static FileResult LoadState(Serializable* sysState){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            return instance->InstanceLoadState(sysState);
        }
        return FileResult::SD_NOT_INITIALIZED;
    }

    static bool ClearState(){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            return instance->InstanceClearState();
        }
        return false;
    }
    /**
     * @brief 
     * 
     * @param config Configuration of type ControllerConfiguration
     * @param fileNameIndex While file name
     */
    static void Save(Serializable* config,PacketType configType){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            instance->InstanceSaveConfigLog(config,configType);
        }
    }

    static bool SaveConfig(Serializable* sysState){
        auto instance=FileManager::Instance();
        if(sdInitialized){
            return instance->InstanceSaveConfig(sysState,PacketType::SAVE_STATE);
        }
        return false;
    }
private:
    void InstanceLoadConfig(Serializable* config,PacketType configType);
    bool InstanceSaveConfig(Serializable* config,PacketType configType);
    void InstanceSaveConfigLog(Serializable* config,PacketType configType);
    bool InstanceClearState();
    FileResult InstanceLoadState(Serializable* config);
private:
    static FileManager* instance;
};