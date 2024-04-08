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
#include "src/Heaters/HeaterTests.hpp"


Controller controller;
// HeaterTests heaterTesting;
void setup(){

/*     heaterTesting.setup_pid();
    Serial.print(F("Free SRAM: "));
    Serial.println(FreeSRAM()); */

    Serial.begin(38400);
    ComHandler::SetSerial(&Serial);
    sdInitialized=true;
    if(!SD.begin(SS)){
        ComHandler::SendErrorMessage(SystemError::SD_INIT_FAILED);
        sdInitialized=false;
    }
    ComHandler::SendSystemMessage(SystemMessage::SD_INIT,MessageType::GENERAL);
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
    //heaterTesting.loop_pid();
    controller.loop();
}

void serialEvent(){
    if(Serial.available()){
        ComHandler::HandleSerial();
    }
    //heaterTesting.handleSerial();
}
