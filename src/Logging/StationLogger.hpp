#pragma once
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <SD.h>
#include "../Communication/ComHandler.hpp"
#include "../constants.h"


#define MSG_BUFFER_SIZE      254

class ComHandler;

class StationLogger{
public:
    static StationLogger* const Instance(){
        if(instance==nullptr){
            instance=new StationLogger;
        }
        return instance;
    }

    static void InitSerial(){
        auto instance=StationLogger::Instance();
        instance->msgBuffer.reserve(MSG_BUFFER_SIZE);
    }

    static void LogInit(LogLevel level,bool printPrefix,const __FlashStringHelper* format,...){
        auto instance=StationLogger::Instance();
        char buffer[64];
        if(printPrefix){
            auto prefix=read_log_prefix(level);
            instance->append_buffers(prefix);
        }
        PGM_P pointer=reinterpret_cast<PGM_P>(format);
        va_list(args);
        va_start(args,format);
        vsnprintf_P(buffer,sizeof(buffer),pointer,args);
        va_end(args);
        instance->append_buffers(buffer);
        instance->print(true);
    }

    static void Log(LogLevel level,bool printPrefix,bool newLine,const __FlashStringHelper* format,...){
        auto instance=StationLogger::Instance();
        char buffer[64];
        if(printPrefix){
            auto prefix=read_log_prefix(level);
            instance->append_buffers(prefix);
        }
        PGM_P pointer=reinterpret_cast<PGM_P>(format);
        va_list(args);
        va_start(args,format);
        vsnprintf_P(buffer,sizeof(buffer),pointer,args);
        va_end(args);
        instance->append_buffers(buffer);
        if(newLine){
            instance->append_buffers("\r\n");
        }
        instance->print(false);
    }

    static void Log(LogLevel level, bool printPrefix,bool newLine,const char* format,...){
        auto instance=StationLogger::Instance();
        char buffer[64];
        if(printPrefix){
            auto prefix=read_log_prefix(level);
            instance->append_buffers(prefix);
        }
        va_list(args);
        va_start(args,format);
        vsnprintf(buffer,sizeof(buffer),format,args);
        va_end(args);
        instance->append_buffers(buffer);
        if(newLine){
            instance->append_buffers("\r\n");
        }
        instance->print(false);
    }

    // static void Log(LogLevel level,bool printPrefix,bool newLine,SystemMessage message,...){
    //     auto instance=StationLogger::Instance();
    //     char buffer[64];
    //     if(printPrefix){
    //         auto prefix=read_log_prefix(level);
    //         instance->append_buffers(prefix);
    //     }
    //     const char* format=read_msg_table(message);
    //     va_list args;
    //     va_start(args, format);
    //     vsnprintf(buffer, sizeof(buffer), format, args);
    //     va_end(args);
    //     instance->append_buffers(buffer);
    //     if(newLine){
    //         instance->append_buffers("\r\n");
    //     }
    //     instance->print(false);
    // }

    // static void PrintFile(){
    //     auto instance=StationLogger::Instance();
    //     if(instance->file){
    //         instance->file=SD.open(read_log_file());
    //         if(instance->file){

    //             while(instance->file.available()){
    //                 Serial.write(instance->file.read());
    //             }
    //             instance->file.close();
    //             instance->file=SD.open(read_log_file(),FILE_WRITE);
    //         }

    //     }
    // }

    void append_buffers(const char* logLine){
        auto addlen=strlen(logLine);
        this->msgBuffer+=logLine;
        //}
    }

    void print(bool init){
        if(init){
            ComHandler::SendInitMessage(this->msgBuffer.c_str());
        }else{
            ComHandler::SendMessage(this->msgBuffer.c_str());
        }
        
        auto len=this->msgBuffer.length();
        this->msgBuffer.remove(0,len);


    }

private:
    static StationLogger* instance;
    String msgBuffer;

};