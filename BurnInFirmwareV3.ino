#include <ArduinoSTL.h>
#include <ArduinoJson.h>
#include <SD.h>
#include "src/Controller/BurnInTimer.hpp"
#include "src/Logging/SerialData.hpp"
#include "src/Heaters/heaters_include.h"
#include "src/Probes/probes_include.h"
#include "src/Configuration/ConfigurationManager.hpp"
#include <Array.h>


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
    MsgPacketSerialize(&doc,probeConfig,ConfigType::PROBE_CONFIG,true);
    serializeJsonPretty(doc,Serial);

    Serial.println("Press s to serialize and d to deserialize");
}

void loop(){
    //heaterTests.loop_pid();
    //if(deserializeLatch){//probe
        JsonDocument doc;
        MsgPacketSerialize(&doc,probeConfig,ConfigType::PROBE_CONFIG,true);
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
        MsgPacketSerialize(&doc,config,ConfigType::HEATER_CONFIG,true);
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