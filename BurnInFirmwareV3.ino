#include <ArduinoSTL.h>
#include <ArduinoJson.h>
#include <PID_v1.h>
#include "src/util.hpp"
#include "src/Controller.hpp"
#include "src/BurnInTimer.hpp"
#include "src/Components_IO/TemperatureSensor.hpp"
#include "src/Components_IO/CurrentSelector.hpp"
#include "src/Components_IO/CurrentSensor.hpp"
#include "src/Components_IO/CurrentSwitch.hpp"
#include "src/Components_IO/Heater.hpp"
#include "src/Components_IO/Probe.hpp"
#include "src/Components_IO/TemperatureSensor.hpp"
#include "src/SerialData.hpp"
#include "src/Tests/SerialOutputTests.hpp"

SerialOutputTests outputTests;

void setup(){
    outputTests.setup();
}

void loop(){
    outputTests.loop();
}



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