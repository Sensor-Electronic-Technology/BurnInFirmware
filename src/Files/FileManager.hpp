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

    static void Initialize(){
        auto instance=FileManager::Instance();
        instance->InstanceInitialize();
    }
    
    static void Load(Serializable* config,PacketType configType){
        auto instance=FileManager::Instance();
        instance->InstanceLoadConfig(config,configType);
    }
    /**
     * @brief 
     * 
     * @param config Configuration of type ControllerConfiguration
     * @param fileNameIndex While file name
     */
    static void Save(Serializable* config,PacketType configType){
        auto instance=FileManager::Instance();
        instance->InstanceSaveConfig(config,configType);
    }
private:
    void InstanceInitialize();
    void InstanceLoadConfig(Serializable* config,PacketType configType);
    void InstanceSaveConfig(Serializable* config,PacketType configType);
private:
    static FileManager* instance;
    JsonDocument doc;
    File file;
    File stateFile;
    WriteBufferingStream fileWriteBuffer{file, 64};
};