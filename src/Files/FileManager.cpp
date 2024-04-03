#include "FileManager.hpp"
#include "../Logging/StationLogger.hpp"

FileManager* FileManager::instance=nullptr;

void FileManager::InstanceInitialize(){
    StationLogger::Log(LogLevel::INFO,true,false,F("Initialize SD Card. Please wait.."));
 
}

void FileManager::InstanceLoadConfig(Serializable* config,PacketType configType){
    JsonDocument doc;
    File file;
    file=SD.open(read_filename(configType));
    if(!file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("Configuration file failed to open.  System will switch to default setup \n Please contact administrator asap!"),
            read_packet_prefix(configType));
        return;
    }
    StationLogger::Log(LogLevel::INFO,true,false,F("File opened, deserializing"));
    doc.clear();
    auto error=deserializeJson(doc,file);
    if(error){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("Failed to load configuration %s.  System will switch to default values. \n Please contact administrator asap!"),
            read_packet_prefix(configType));
        file.close();
        return;
    }
    //serializeJsonPretty(instance->doc,Serial);
    config->Deserialize(doc);
    file.close();
    doc.clear();
}

FileResult FileManager::InstanceLoadState(Serializable* sysState){
    JsonDocument doc;
    File file;
    StationLogger::Log(LogLevel::INFO,true,false,F("Checking if state file exists"));
    if(!SD.exists(read_filename(PacketType::SAVE_STATE))){
        return FileResult::DOES_NOT_EXIST;
    }
    StationLogger::Log(LogLevel::INFO,true,false,F("State file exists, opening...."));
    file=SD.open(read_filename(PacketType::SAVE_STATE));
    if(!file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("Configuration file failed to open. Cannot continue from saved state. Manually start test"));
        return FileResult::FAILED_TO_OPEN;
    }
    StationLogger::Log(LogLevel::INFO,true,false,F("Saved state file opened, deserializing..."));
    doc.clear();
    auto error=deserializeJson(doc,file);
    if(error){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("State Deserialization Failed. \n Cannot continue from saved state. \n Manually start test"));
        file.close();
        return FileResult::DESERIALIZE_FAILED;
    }
    sysState->Deserialize(doc);
    file.close();
    doc.clear();
    return FileResult::LOADED;
}

void FileManager::InstanceSaveConfig(Serializable* config,PacketType configType){
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};

    SD.remove(read_filename(configType));//overwrite file
    file=SD.open(read_filename(configType),FILE_WRITE);
    if(!file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                F("Configuration file failed to open.  Please save values and restart the system"));
        return;
    }
    doc.clear();
    config->Serialize(&doc,true);
    if(serializeJsonPretty(doc,fileWriteBuffer)==0){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                F("Configuration save failed. Changed will be lost on restart.\n Suggest manually enter changes"));
        file.close();
    }
    switch(configType){
        case PacketType::HEATER_CONFIG:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Heater configuration saved"));
            break;
        }
        case PacketType::PROBE_CONFIG:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Probe configuration saved"));
            break;
        }
        case PacketType::SYSTEM_CONFIG:{
            StationLogger::Log(LogLevel::INFO,true,false,F("Station configuration saved"));
            break;
        }
        default:{
            StationLogger::Log(LogLevel::INFO,true,false,F("File Saved"));
            break;
        }
    }
    fileWriteBuffer.flush();
    file.close();
    doc.clear();
}
