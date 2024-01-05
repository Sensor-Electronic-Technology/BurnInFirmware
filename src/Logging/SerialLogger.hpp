#pragma once
#include <SD.h>
#include <avr/pgmspace.h>

enum MessageType{
    Error,
    SystemMessage,
    Data
};


class SerialLogger{
public:
    static SerialLogger* const Instance(){
        if(instance==nullptr){
            instance=new SerialLogger;
        }
        return instance;
    }

    static void Log(MessageType messageType,const char *longLine){
        auto instance=SerialLogger::Instance();
        switch(messageType){
            case MessageType::Data:{
                break;
            }
            case MessageType::SystemMessage:{
                break;
            }
            case MessageType::Error:{
                break;
            }
        }
    }

    void print(const __FlashStringHelper* format,...){
        
        char buffer[128];
        PGM_P pointer=reinterpret_cast<PGM_P>(format);
        va_list args;
        va_start(args,format);
        vsnprintf_P(buffer,sizeof(buffer),pointer,args);
        va_end(args);
        this->serialLog->println();


    }
private:
    static SerialLogger* instance;
    File* logFile;
    Print* serialLog;
};