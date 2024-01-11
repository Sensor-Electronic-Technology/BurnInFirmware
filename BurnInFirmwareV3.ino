#include <ArduinoJson.h>
#include <SD.h>
//#include "src/Controller/BurnInTimer.hpp"
#include "src/constants.h"
#include "src/Communication/Serializable.hpp"
#include "src/Heaters/heaters_include.h"
#include "src/Probes/probes_include.h"
#include "src/Configuration/ConfigurationManager.hpp"
#include "src/Communication/ComHandler.hpp"
#include "src/Logging/StationLogger.hpp"
#include <Array.h>
#include <ArxContainer.h>

#define ARDUINOJSON_DEFAULT_NESTING_LIMIT 50

template< typename Tag, typename Tag::type M >
  struct AccessMember{ 
    friend typename Tag::type get( Tag ){ return M; }
};



unsigned long lastCheck=0;
unsigned long deadLine=5000;

void testing(){
    if(!SD.begin(SS)){
        StationLogger::Log(LogLevel::CRITICAL_ERROR,true,true,"Failed to open SD card. Configurations cannot be read");
        while(true){ }
    }
    StationLogger::InitFile();
    
    StationLogger::Log(LogLevel::ERROR,true,true,SystemMessage::CHECKING_RUNNING_TEST);
    delay(5000);
    Serial.println("Reading File");
    StationLogger::PrintFile();
    Serial.println("Finished");    
    
}

int count=0;
JsonDocument doc;
bool serialEventReady=false;

void setup(){
    Serial.begin(38400);
    while(!Serial){}
    ComHandler::SetSerial(&Serial);
    StationLogger::SetSerial(&Serial);
    StationLogger::Log(LogLevel::INFO,true,false,"Connected, send data");
    lastCheck=millis();
    String prefixStr;
    for(int i=0;i<5;i++){
        prefixStr+=read_packet_prefix(i);
        prefixStr+=",";
    }
    StationLogger::Log(LogLevel::INFO,true,false,"Prefix List.. %s",prefixStr.c_str());
    serialFlush();
    ComHandler::EnableSerialEvent();
}

void loop(){
    if(millis()-lastCheck>=deadLine){
        StationLogger::Log(LogLevel::INFO,true,false,"Sending.. %d",count++);
        lastCheck=millis();
    }
}

void serialEvent(){
    if(Serial.available()){
        ComHandler::HandlerSerial();
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