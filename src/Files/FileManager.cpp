#include "FileManager.hpp"

FileManager* FileManager::instance=nullptr;

bool FileManager::InstanceLoadConfig(Serializable* config,ConfigType configType){
    JsonDocument doc;
    //File file;
    File file;
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));
    //file=SD.open(filename);
    file.open(filename,FILE_READ);
    if(!file){
        ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED_FILE,filename);
        return false;
    }
    doc.clear();
    auto error=deserializeJson(doc,file);
    if(error){
        ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED_FILE,filename);
        file.close();
        return false;
    }
    config->Deserialize(doc);
    file.close();
    doc.clear();
}

FileResult FileManager::InstanceLoadState(Serializable* sysState){
    JsonDocument doc;
    //File file;
    File file;
    char filename[BUFFER_SIZE];
    strcpy_P(filename,strFile_04);
    Serial.print(F("State File: "));
    Serial.println(filename);
    if(!this->sd.exists(filename)){
        Serial.println(F("File does not exist"));
        return FileResult::DOES_NOT_EXIST;
    }
    file.open(filename,FILE_READ);
    if(!file){
        return FileResult::FAILED_TO_OPEN;
    }
    doc.clear();
    auto error=deserializeJson(doc,file);
    if(error){
        file.close();
        return FileResult::DESERIALIZE_FAILED;
    }
    sysState->Deserialize(doc);
    file.close();
    doc.clear();
    return FileResult::LOADED;
}

bool FileManager::InstanceSaveConfig(Serializable* config,ConfigType configType){
    JsonDocument doc;
    FsFile file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));  
    if(this->sd.exists(filename)){
        this->sd.remove(filename);//overwrite file  
    }
    file.open(filename,FILE_WRITE);
    if(!file){
        ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED_FILE,filename);
        return false;
    }
    config->Serialize(&doc,true);
    if(serializeJsonPretty(doc,fileWriteBuffer)==0){
        file.close();
        ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED_FILE,filename);
        return false;
    }
    fileWriteBuffer.flush();
    file.close();
    doc.clear();
    return true;
}

bool FileManager::InstanceSaveState(Serializable* config){
    JsonDocument doc;
    FsFile file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    char filename[BUFFER_SIZE];
    strcpy_P(filename,strFile_04);    
    if(this->sd.exists(filename)){
        this->sd.remove(filename);//overwrite file  
    }
    file.open(filename,FILE_WRITE);
    if(!file){
        ComHandler::SendErrorMessage(SystemError::SAVED_STATE_FAILED,filename);
        return;
    }
    config->Serialize(&doc,true);
    if(serializeJsonPretty(doc,fileWriteBuffer)==0){
        file.close();
        ComHandler::SendErrorMessage(SystemError::SAVED_STATE_FAILED,filename);
        return;
    }
    fileWriteBuffer.flush();
    file.close();
    doc.clear();
}

bool FileManager::InstanceFormatCard(){
    HeaterControllerConfig heaterConfig;
    ProbeControllerConfig probeConfig;
    ControllerConfig controllerConfig;
    bool okay=true;
    okay&=this->LoadConfiguration(&heaterConfig,ConfigType::HEATER_CONFIG);
    okay&=this->LoadConfiguration(&probeConfig,ConfigType::PROBE_CONFIG);
    okay&=this->LoadConfiguration(&controllerConfig,ConfigType::SYSTEM_CONFIG);
    if(!okay){
        ComHandler::SendErrorMessage(SystemError::SD_FORMAT_FAILED,this->sd.sdErrorCode(),this->sd.sdErrorData());
        return false;
    }
    auto success=this->sd.format(&Serial);
    if(!success){
        ComHandler::SendErrorMessage(SystemError::SD_FORMAT_FAILED,this->sd.sdErrorCode(),this->sd.sdErrorData());
        return false;
    }
    this->SaveConfiguration(&heaterConfig,ConfigType::HEATER_CONFIG);
    this->SaveConfiguration(&probeConfig,ConfigType::PROBE_CONFIG);
    this->SaveConfiguration(&controllerConfig,ConfigType::SYSTEM_CONFIG);
    ComHandler::SendSystemMessage(SystemMessage::SD_FORMATTED,MessageType::NOTIFY);
    return true;
}

bool FileManager::InstanceClearState(){
    char filename[BUFFER_SIZE];
    strcpy_P(filename,strFile_04);
    if(!this->sd.exists(filename)){
        return true;
    }
    return this->sd.remove(filename);
}
