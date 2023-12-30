#include <ArduinoSTL.h>
#include <ArduinoJson.h>
#include <PID_v1.h>
#include "src/util.hpp"
#include "src/Controller.hpp"
#include "src/BurnInTimer.hpp"
#include "src/Components_IO/includes_io.hpp"
#include "src/SerialData.hpp"
#include "src/Tests/SerialOutputTests.hpp"
#include "src/Tests/SerialInputTests.hpp"
#include <Array.h>

//SerialOutputTests outputTests;
SerialInputTests inputTests;

void setup(){
    //outputTests.setup();
    inputTests.setup();
    
}

void loop(){
    //outputTests.loop();
    inputTests.loop();
}

void serialEvent(){
    inputTests.HandleSerial();
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