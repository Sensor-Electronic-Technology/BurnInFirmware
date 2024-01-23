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

enum FileResult{
    DOES_NOT_EXIST,
    FAILED_TO_OPEN,
    DESERIALIZE_FAILED,
    LOADED,
    SAVED,
    FAILED_TO_SERIALIZE,
    FAILED_TO_SAVE
};

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

    static FileResult LoadState(Serializable* sysState){
        auto instance=FileManager::Instance();
        return instance->InstanceLoadState(sysState);
    }

    static bool ClearState(){
        auto filename=read_filename(PacketType::SAVE_STATE);
        return SD.remove(filename);
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
    FileResult InstanceLoadState(Serializable* config);
private:
    static FileManager* instance;
    JsonDocument doc;
    File file;
    File stateFile;
    WriteBufferingStream fileWriteBuffer{file, 64};
};