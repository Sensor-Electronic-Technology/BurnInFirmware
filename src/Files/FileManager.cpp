#include "FileManager.hpp"
#include "../Logging/StationLogger.hpp"

FileManager* FileManager::instance=nullptr;

void FileManager::InstanceInitialize(){
    StationLogger::Log(LogLevel::INFO,true,false,F("Initialize SD Card. Please wait.."));
 
}

void FileManager::InstanceLoadConfig(Serializable* config,PacketType configType){
    auto fileName=read_filename(configType);
    this->file=SD.open(fileName);
    if(!this->file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("Configuration file failed to open.  System will switch to default setup \n Please contact administrator asap!"),
            read_packet_prefix(configType));
        return;
    }
    StationLogger::Log(LogLevel::INFO,true,false,F("File opened, deserializing"));
    this->doc.clear();
    auto error=deserializeJson(this->doc,this->file);
    if(error){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("Failed to load configuration %s.  System will switch to default values. \n Please contact administrator asap!"),
            read_packet_prefix(configType));
        this->file.close();
        return;
    }
    //serializeJsonPretty(instance->doc,Serial);
    config->Deserialize(instance->doc);
    this->file.close();
    this->doc.clear();
}

FileResult FileManager::InstanceLoadState(Serializable* sysState){
    auto fileName=read_filename(PacketType::SAVE_STATE);
    StationLogger::Log(LogLevel::INFO,true,false,F("Checking if state file exists"));
    if(!SD.exists(fileName)){
        return FileResult::DOES_NOT_EXIST;
    }
    StationLogger::Log(LogLevel::INFO,true,false,F("State file exists, opening...."));
    this->file=SD.open(fileName);
    if(!this->file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("Configuration file failed to open. Cannot continue from saved state. Manually start test"));
        return FileResult::FAILED_TO_OPEN;
    }
    StationLogger::Log(LogLevel::INFO,true,false,F("Saved state file opened, deserializing..."));
    this->doc.clear();
    auto error=deserializeJson(this->doc,this->file);
    if(error){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
            F("State Deserialization Failed. \n Cannot continue from saved state. \n Manually start test"));
        this->file.close();
        return FileResult::DESERIALIZE_FAILED;
    }
    sysState->Deserialize(instance->doc);
    this->file.close();
    this->doc.clear();
    return FileResult::LOADED;
}

void FileManager::InstanceSaveConfig(Serializable* config,PacketType configType){
    auto fileName=read_filename(configType);
    SD.remove(fileName);//overwrite file
    this->file=SD.open(fileName,FILE_WRITE);
    if(!this->file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                F("Configuration file failed to open.  Please save values and restart the system"));
        return;
    }
    this->doc.clear();
    config->Serialize(&this->doc,true);
    if(serializeJsonPretty(this->doc,this->fileWriteBuffer)==0){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                F("Configuration save failed. Changed will be lost on restart.\n Suggest manually enter changes"));
        this->file.close();
        
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
    this->fileWriteBuffer.flush();
    this->file.close();
    this->doc.clear();
}
