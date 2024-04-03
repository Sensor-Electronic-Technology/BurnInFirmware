#pragma once
#include "../SerialData.hpp"
#include <ArduinoJson.h>

#define ComPeriod 2000
class SerialOutputTests{
private:
    StaticJsonDocument<384> doc;
    SerialDataOutput output;
    unsigned long lastCheck=0;
public:
    void setup(){
        Serial.begin(38400);
        //while(!Serial){ }
        lastCheck=millis();
        output.Serialize(&doc,true);
    }

    void loop(){
        if(millis()-lastCheck>=ComPeriod){
            lastCheck=millis();
            for(uint8_t i=0;i<6;i++){
                output.probeRuntimes[i]+=5;
            }
            output.Serialize(&doc,false);
            serializeJson(doc,Serial);
        }
    }

};