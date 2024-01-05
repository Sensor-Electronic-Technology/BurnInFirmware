#pragma once
//#include <SD.h>
#include <avr/pgmspace.h>


class SerialLogger{
public:
    static SerialLogger* const Instance(){
        if(instance==nullptr){
            instance=new SerialLogger;
        }
        return instance;
    }

    static void LogMessage(){
        
    }
private:
    static SerialLogger* instance;
    //File logFile;
};