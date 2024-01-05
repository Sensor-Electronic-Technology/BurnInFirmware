#pragma once
#include <SD.h>
#include <StreamUtils.h>
#include <avr/pgmspace.h>

enum MessageType{
    Error,
    SystemMessage,
    ConfigData,
    SystemData
};


template<class T> class LogCarrier{
public:
    char *pre;
    unsigned long time;
    T  data;
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
            case MessageType::SystemData:{
                break;
            }
            case MessageType::ConfigData:{
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

    static void SetPrint(Print *printer){
        auto instance=SerialLogger::Instance();
        instance->serialLog=printer;
    }

    static void SetLoggerFile(File* loggerFile){
        auto instance=SerialLogger::Instance();
        instance->logFile=loggerFile;
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

    void printPrefix(MessageType messageType,bool include_time){
        char buffer[16];
        if(include_time){

        }
        switch(messageType){
            case MessageType::SystemData:{
                break;
            }
            case MessageType::ConfigData:{
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
private:
    static SerialLogger* instance;
    File* logFile=nullptr;
    WriteBufferingStream fileBuffer{*logFile,64};
    Print* serialLog=nullptr;
    bool printInitialized;
    bool fileInitialized;

};