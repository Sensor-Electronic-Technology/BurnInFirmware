#include <ArduinoJson.h>
#include <SD.h>
#include "src/constants.h"
#include "src/Communication/ComHandler.hpp"
#include "src/Heaters/heater_constants.h"
#include "src/Heaters/HeaterConfiguration.hpp"
#include "src/Heaters/heaters_include.h"
#include "src/Probes/probes_include.h" 
#include "src/Files/FileManager.hpp"
#include "src/Controller/Controller.hpp"
#include "src/StressTest/TestController.hpp" 
#include "src/free_memory.h"


Controller controller;
void setup(){
    Serial.begin(38400);
    bool sdInit=false;
    ComHandler::SetSerial(&Serial);
    if(!SD.begin(SS)){
        ComHandler::SendErrorMessage(SystemError::SD_INIT_FAILED);
        sdInit=true;
    }
    FileManager::SetInitialized(sdInit);
    Serial.print(F("Free SRAM: "));
    Serial.println(FreeSRAM());
    //EEPROM_write(VER_ADDR,"V1.0.0");
    EEPROM_read(ID_ADDR,StationId);
    randomSeed(analogRead(0));
    ComHandler::SetSerial(&Serial);
    controller.LoadConfigurations();
    controller.SetupComponents();
    ComHandler::EnableSerialEvent();
    Serial.print(F("Free SRAM: "));
    Serial.println(FreeSRAM());
}

void loop(){
    controller.loop();
}

void serialEvent(){
    if(Serial.available()){
        ComHandler::HandleSerial();
    }   
}
