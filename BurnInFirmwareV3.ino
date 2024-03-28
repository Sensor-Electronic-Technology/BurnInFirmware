#include <ArduinoJson.h>
#include <SD.h>
#include <Array.h>
#include "src/constants.h"
#include "src/Heaters/heater_constants.h"
#include "src/Communication/ComHandler.hpp"
#include "src/Heaters/HeaterConfiguration.hpp"
#include "src/Heaters/heaters_include.h"
#include "src/Probes/probes_include.h"
#include "src/Files/FileManager.hpp"
#include "src/Communication/ComHandler.hpp"
#include "src/Logging/StationLogger.hpp"
#include "src/Controller/StationState.hpp"
#include "src/Controller/Controller.hpp"
#include "src/free_memory.h"



unsigned long lastCheck=0;
unsigned long deadLine=3000;


Controller controller;


int count=0;
JsonDocument doc;
bool serialEventReady=false;

void setup(){
    Serial.begin(38400);
    //EEPROM_write(VER_ADDR,"V1.0.0");
    EEPROM_read(ID_ADDR,StationId);
    randomSeed(analogRead(0));
    ComHandler::SetSerial(&Serial);
    StationLogger::InitSerial();
    if(!SD.begin(SS)){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,true,F("Failed to open SD card. Configurations cannot be read"));
        while(true){ }
    }
    StationLogger::LogInit(LogLevel::INFO,true,F("SD Opened"));
    StationLogger::LogInit(LogLevel::INFO,true,F("Connected, send data"));
    lastCheck=millis();
    controller.LoadConfigurations();
    controller.SetupComponents();
    ComHandler::EnableSerialEvent();
    //String sid=id;
    StationLogger::LogInit(LogLevel::INFO,true,F("StationId: %s"),StationId);
}

void loop(){
    // if(millis()-lastCheck>=deadLine){
    //     // StationLogger::Log(LogLevel::INFO,true,false,F("Free Memory: %d"),FreeSRAM());
    //    // EEPROM_read(VER_ADDR,FirmwareVersion);
    //     //StationLogger::Log(LogLevel::INFO,true,false,F("Version: %s"),FirmwareVersion);
    //     ComHandler::SendOutVersion();
    //      lastCheck=millis();

    // }
    controller.loop();
}

void serialEvent(){
    if(Serial.available()){
        ComHandler::HandleSerial();
    }   
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}

/*
//SerialOutputTests outputTests;
//SerialInputTests inputTests;
unsigned long lastWindowCheck=0;
unsigned long windowSize=1000;

HeaterControllerConfig config;
ProbeControllerConfig probeConfig;
HeaterController controller(config);
HeaterTests heaterTests(&controller);

bool serializeLatch=false,deserializeLatch=false;

void setup(){
    //heaterTests.setup_pid();
    Serial.begin(38400);
    //while(!Serial){  }
    Serial.println("Opening SD Card");
    if(!SD.begin(SS)){
        Serial.println("Error: Failed to open sd card");
        while(true){}
    }
    // MessagePacket<HeaterControllerConfig> msg;
    JsonDocument doc;
    MsgPacketSerializer(&doc,probeConfig,PacketType::PROBE_CONFIG,true);
    serializeJsonPretty(doc,Serial);

    Serial.println("Press s to serialize and d to deserialize");
}

void loop(){
    //heaterTests.loop_pid();
    //if(deserializeLatch){//probe
        JsonDocument doc;
        MsgPacketSerializer(&doc,probeConfig,PacketType::PROBE_CONFIG,true);
        //Serial.print('~');
        serializeJson(doc,Serial);
        //Serial.print('!');
        Serial.println();
        delay(2000);
        // Serial.println("Serializing doc");
        // ConfigurationManager::LoadConfig(probeConfig,PROBE_CONFIG_INDEX);
        // Serial.println("Serializing Done, Press s to serialize and d to deserialize");
    //}
    //if(serializeLatch){//heater
        doc.clear();
        MsgPacketSerializer(&doc,config,PacketType::HEATER_CONFIG,true);
        //Serial.print('~');
        serializeJson(doc,Serial);
        //Serial.print('!');
        Serial.println();
        serializeLatch=false;
        delay(2000);
        // Serial.println("Serializing doc");
        // ConfigurationManager::SaveConfig(probeConfig,PROBE_CONFIG_INDEX);
        // Serial.println("Serializing Done, Press s to serialize and d to deserialize");
    //}
}

void serialEvent(){
    //heaterTests.handleSerial_pid();
    if(Serial.available()){
        char b = Serial.read();
        Serial.flush();  
        if(b=='h'){
            serializeLatch=true;
        }else if(b=='p'){
            deserializeLatch=true;
        }
    }
}
*/

// void serialEvent(){
//     //heaterTests.handleSerial_pid();
//     if(Serial.available()){
//         char b = Serial.read();
//         Serial.flush();  
//         if(b=='s'){
//             serializeLatch=true;
//         }else if(b=='d'){
//             deserializeLatch=true;
//         }
//     }
// }

/*
void serialEvent(){
    if(Serial.available()){
        char b = Serial.read();
        Serial.flush();  
        if(b=='s'){
            Serial.println("Tuning Starting");
            controller.StartTuning();
        }else if(b=='t'){
            Serial.println("Stopping AutoTune");
            controller.StopTuning();
        }
    }
    //inputTests.HandleSerial();
}*/





// #define ComPeriod 2000

// unsigned long lastCheck=0;
// StaticJsonDocument<384> doc;
// SerialDataOutput output;

// void setup(){
//     Serial.begin(38400);
//     //while(!Serial){ }
//     lastCheck=millis();
//     output.Serialize(&doc,true);
// }

// void loop(){
//     if(millis()-lastCheck>=ComPeriod){
//         lastCheck=millis();
//         for(int i=0;i<6;i++){
//             output.probeRuntimes[i]+=5;
//         }
//         output.Serialize(&doc,false);
//         serializeJson(doc,Serial);
//     }
// }