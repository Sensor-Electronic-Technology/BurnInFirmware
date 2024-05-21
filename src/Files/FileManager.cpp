#include "FileManager.hpp"

FileManager* FileManager::instance=nullptr;

void FileManager::InstanceLoadConfig(Serializable* config,ConfigType configType){
    JsonDocument doc;
    File file;
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));
    file=SD.open(filename);
    if(!file){
        //ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED_FILE,filename);
        return;
    }
    doc.clear();
    auto error=deserializeJson(doc,file);
    if(error){
        //ComHandler::SendErrorMessage(SystemError::CONFIG_LOAD_FAILED_FILE,filename);
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
    strcpy_P(filename,strFile_04);
    Serial.print(F("State File: "));
    Serial.println(filename);
     if(!SD.exists(filename)){
        //Serial.println(F("File does not exist"));
        return FileResult::DOES_NOT_EXIST;
    }
    //Serial.println(F("File exists"));
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

bool FileManager::InstanceSaveConfig(Serializable* config,ConfigType configType){
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    char filename[BUFFER_SIZE];
    strcpy_P(filename,read_filename(configType));    
    SD.remove(filename);//overwrite file
    file=SD.open(filename,FILE_WRITE);
    if(!file){
        return false;
    }
    config->Serialize(&doc,true);
    if(serializeJsonPretty(doc,fileWriteBuffer)==0){
        file.close();
        return false;
    }
    fileWriteBuffer.flush();
    file.close();
    doc.clear();
    return true;
}

bool FileManager::InstanceSaveState(Serializable* config){
    JsonDocument doc;
    File file;
    WriteBufferingStream fileWriteBuffer{file, 64};
    char filename[BUFFER_SIZE];
    strcpy_P(filename,strFile_04);    
    SD.remove(filename);//overwrite file
    file=SD.open(filename,FILE_WRITE);
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

bool FileManager::InstanceClearState(){
    char filename[BUFFER_SIZE];
    strcpy_P(filename,strFile_04);
    return SD.remove(filename);
}
