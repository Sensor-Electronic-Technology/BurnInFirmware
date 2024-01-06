#pragma once
#include "../SerialData.hpp"
#include <ArduinoJson.h>

#define ComPeriod 2000
class SerialInputTests{
private:
    StaticJsonDocument<512> doc;
    SerialDataOutput input;
    unsigned long lastCheck=0;
public:
    void setup(){
        Serial.begin(38400);
        //while(!Serial){ }
        lastCheck=millis();
        //input.Serialize(&doc,true);
    }

    void loop(){
        if(millis()-lastCheck>ComPeriod){
            lastCheck=millis();
            for(int i=0;i<6;i++){
                Serial.print("[");Serial.print(i);Serial.print("]=");
                Serial.print(input.probeRuntimes[i]);
            }
            Serial.println();
        }
    }

    void HandleSerial(){
        if(Serial.available()){
            // Serial.readStringUntil('}');
            auto err=deserializeJson(doc,Serial);
            if (err) {
                Serial.print(F("deserializeJson() failed: "));
                Serial.println(err.f_str());
                return;
            }
            input.DeSerialize(&doc);
            serializeJson(doc,Serial);
            //Serial.println(input.Serialize())

        }
    }

};