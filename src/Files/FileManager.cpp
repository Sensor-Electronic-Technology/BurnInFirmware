#include "FileManager.hpp"
#include "../Logging/StationLogger.hpp"

FileManager* FileManager::instance=nullptr;

void FileManager::InstanceLoadConfig(Serializable* config,PacketType configType){
        auto fileName=read_filename(configType);
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

void FileManager::InstanceSaveConfig(Serializable* config,PacketType configType){
    auto fileName=read_filename(configType);
    SD.remove(fileName);//overwrite file
    this->file=SD.open(fileName,FILE_WRITE);
    if(!this->file){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                "Configuration file failed to open.  Please save values and restart the system");
        return;
    }
    this->doc.clear();
    config->Serialize(&this->doc,true);
    if(serializeJsonPretty(this->doc,this->fileWriteBuffer)==0){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,
                "Configuration save failed. Changed will be lost on restart.\n Suggest manually enter changes");
    }
    //TODO: test this
    //ComHandler::MsgPacketSerializer(*config,configType);
    StationLogger::Log(LogLevel::INFO,true,false,"Configuration file should be saved");
    serializeJsonPretty(this->doc,Serial);
    this->fileWriteBuffer.flush();
    this->file.close();
}
