#include "FileManager.hpp"

FileManager* FileManager::instance=nullptr;

void FileManager::InstanceLoadConfig(Serializable* config,PacketType configType){
    JsonDocument doc;
    File file;
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));
    file=SD.open(filename);
    if(!file){
        ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED_FILE,filename);
        return;
    }
    doc.clear();
    auto error=deserializeJson(doc,file);
    if(error){
        ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED_FILE,filename);
        file.close();
        return;
    }
    config->Deserialize(doc);
    file.close();
    doc.clear();
}

FileResult FileManager::InstanceLoadState(Serializable* sysState){
    JsonDocument doc;
    File file;
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(PacketType::SAVE_STATE));
     if(!SD.exists(filename)){
        Serial.println(F("File does not exist"));
        return FileResult::DOES_NOT_EXIST;
    }
    Serial.println(F("File exists"));
    file=SD.open(filename);
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

void FileManager::InstanceSaveConfigLog(Serializable* config,PacketType configType){
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));    
    SD.remove(filename);//overwrite file
    file=SD.open(filename,FILE_WRITE);
    if(!file){
        ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED_FILE,filename);
        // ComHandler::SendFileMsg(FileResult::FAILED_TO_OPEN,F("Configuration file failed to open.  Please save values and restart the system"));
        return;
    }
    doc.clear();
    config->Serialize(&doc,true);
    if(serializeJsonPretty(doc,fileWriteBuffer)==0){
        file.close();
        ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED_FILE,filename);
        // ComHandler::SendFileMsg(FileResult::FAILED_TO_SAVE,F("Configuration save failed. Changed will be lost on restart.\n Suggest manually enter changes"));
        return;
    }
    fileWriteBuffer.flush();
    file.close();
    doc.clear();
}

bool FileManager::InstanceSaveConfig(Serializable* config,PacketType configType){
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));    
    SD.remove(filename);//overwrite file
    file=SD.open(filename,FILE_WRITE);
    if(!file){
        ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED_FILE,filename);
        // ComHandler::SendFileMsg(FileResult::FAILED_TO_OPEN,F("Configuration file failed to open.  Please save values and restart the system"));
        return false;
    }
    doc.clear();
    config->Serialize(&doc,true);
    if(serializeJsonPretty(doc,fileWriteBuffer)==0){
        file.close();
        ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED_FILE,filename);
        // ComHandler::SendFileMsg(FileResult::FAILED_TO_SAVE,F("Configuration save failed. Changed will be lost on restart.\n Suggest manually enter changes"));
        return false;
    }
    fileWriteBuffer.flush();
    file.close();
    doc.clear();
    return true;
}

bool FileManager::InstanceClearState(){
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(PacketType::SAVE_STATE));
    return SD.remove(filename);
}
