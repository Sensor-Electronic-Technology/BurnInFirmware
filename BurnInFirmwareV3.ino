#include <ArduinoJson.h>
#include <SD.h>
#include "src/constants.h"
#include "src/Heaters/heater_constants.h"
#include "src/Communication/ComHandler.hpp"
#include "src/Heaters/HeaterConfiguration.hpp"
#include "src/Heaters/heaters_include.h"
#include "src/Probes/probes_include.h"
#include "src/Files/FileManager.hpp"
#include "src/Communication/ComHandler.hpp"
#include "src/Logging/StationLogger.hpp"
#include "src/Controller/Controller.hpp"
#include "src/StressTest/TestController.hpp"
#include "src/free_memory.h"

unsigned long lastCheck=0;
unsigned long deadLine=3000;

Controller controller;
void setup(){
    Serial.begin(38400);
    //EEPROM_write(VER_ADDR,"V1.0.0");
    EEPROM_read(ID_ADDR,StationId);
    randomSeed(analogRead(0));
    ComHandler::SetSerial(&Serial);
    StationLogger::InitSerial();
    bool sdInit=false;
    if(!SD.begin(SS)){
        ComHandler::SendErrorMessage(SystemError::SD_INIT_FAILED);
        sdInit=true;
    }
    FileManager::SetInitialized(sdInit);
    // lastCheck=millis();
    controller.LoadConfigurations();
    controller.SetupComponents();
    ComHandler::EnableSerialEvent();

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


// HeaterControllerConfig heatersConfig;
// ProbeControllerConfig probesConfig;
// ControllerConfig controllerConfig;


// HeaterController heaterController;
// ProbeController probeController;
// TestController testController;

// File file;
// JsonDocument doc;

// unsigned int initial=0;
// unsigned int heaterSize=0;
// unsigned int probeSize=0;
// unsigned int controlSize=0;

// void setup(){
//     Serial.begin(38400);
//     while(!Serial){_NOP();}

//     if(!SD.begin(SS)){
//         Serial.println(F("Failed to open SD card. Configurations cannot be read"));
//         while(true){ }
//     }

//     Serial.println(F("SD Opened"));
//     Serial.print(F("Initial SRAM: "));
//     initial=FreeSRAM();
//     Serial.println(initial);
//     file=SD.open("/hConfigs.txt");
//     if(!file){
//         Serial.println(F("Failed to open file"));
//         file.close();
//     }
//     deserializeJson(doc,file);
//     heatersConfig.Deserialize(doc);
//     //FileManager::Load(&heatersConfig,PacketType::HEATER_CONFIG);
//     file.close();
//     doc.clear();
    
//     heaterController.Setup(heatersConfig);
//     heaterController.Initialize();
//     Serial.print(F("SRAM After Heaters: "));
//     Serial.print(FreeSRAM());
//     Serial.print(F(" HeaterSize: "));
//     heaterSize=initial-FreeSRAM();
//     initial=FreeSRAM();
//     Serial.println(heaterSize);
//     file=SD.open("/pConfigs.txt");
//     if(!file){
//         Serial.println(F("Failed to open file"));
//         file.close();
//     }
//     deserializeJson(doc,file);
//     probesConfig.Deserialize(doc);
//     file.close();
//     doc.clear();
//     probeController.Setup(probesConfig);
//     probeController.Initialize();
//     //FileManager::Load(&probesConfig,PacketType::PROBE_CONFIG);
//     Serial.print(F("SRAM After Probes: "));
//     Serial.print(FreeSRAM());
//     Serial.print(F(" ProbeSize: "));
//     probeSize=initial-FreeSRAM();
//     initial=FreeSRAM();
//     Serial.println(probeSize);
    
//     file=SD.open("/sConfig.txt");
//     if(!file){
//         Serial.println(F("Failed to open file"));
//         file.close();
//     }
//     deserializeJson(doc,file);
//     controllerConfig.Deserialize(doc);
//     file.close();
//     doc.clear();
//     testController.SetConfig(controllerConfig.burnTimerConfig);
//     //FileManager::Load(&controllerConfig,PacketType::SYSTEM_CONFIG);
//     Serial.print(F("SRAM After Control: "));
//     Serial.print(FreeSRAM());
//     Serial.print(F(" Control: "));
//     probeSize=initial-FreeSRAM();
//     Serial.println(probeSize);
//     Serial.print(F("Final SRAM: "));
//     Serial.println(FreeSRAM());
// }

// void loop(){
//     delay(1000);
//     heaterController.loop();
//     probeController.loop();
//     testController.loop();
//     Serial.println(FreeSRAM());
// }*/


/*void setup(){
    // Serial.begin(38400);
    // //EEPROM_write(VER_ADDR,"V1.0.0");
    // EEPROM_read(ID_ADDR,StationId);
    // randomSeed(analogRead(0));
    // ComHandler::SetSerial(&Serial);
    // StationLogger::InitSerial();
    // if(!SD.begin(SS)){
    //     StationLogger::Log(LogLevel::CRITICAL_ERROR,true,true,F("Failed to open SD card. Configurations cannot be read"));
    //     while(true){ }
    // }
    // StationLogger::LogInit(LogLevel::INFO,true,F("SD Opened"));
    // StationLogger::LogInit(LogLevel::INFO,true,F("Connected, send data"));
    // lastCheck=millis();
    // controller.LoadConfigurations();
    // controller.SetupComponents();
    // ComHandler::EnableSerialEvent();
    // //String sid=id;
    // StationLogger::LogInit(LogLevel::INFO,true,F("StationId: %s"),StationId);
    Serial.begin(38400);
    Serial.println(F("Starting"));
}

void loop(){
    // if(millis()-lastCheck>=deadLine){
    //     // StationLogger::Log(LogLevel::INFO,true,false,F("Free Memory: %d"),FreeSRAM());
    //    // EEPROM_read(VER_ADDR,FirmwareVersion);
    //     //StationLogger::Log(LogLevel::INFO,true,false,F("Version: %s"),FirmwareVersion);
    //     ComHandler::SendOutVersion();
    //      lastCheck=millis();

    // }
    //controller.loop();
    delay(1000);
    Serial.println(FreeSRAM());
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
*/