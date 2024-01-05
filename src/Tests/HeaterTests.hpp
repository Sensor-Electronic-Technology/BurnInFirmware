#pragma once
#include "../Configuration/HeaterConfiguration.hpp"
#include "../Components_IO/HeaterController.hpp"

class HeaterTests{
public:
    HeaterTests(HeaterController *_controller):controller(_controller){   }

    void setup_pid(){
        Serial.begin(38400);
        while(!Serial){}
        Serial.println("Initializing");
        controller->Initialize();
        //Serial.println("Press s to start Tuning and t to manually stop");
        Serial.println("Press s to start heating and t to manually stop");
    }

    void loop_pid(){
        controller->loop();
    }

    void handleSerial_pid(){
        if(Serial.available()){
            char b = Serial.read();
            Serial.flush();  
            if(b=='s'){
                Serial.println("Heater Output On");
                controller->TurnOn();
            }else if(b=='t'){
                Serial.println("Heater Output Off");
                controller->TurnOff();
            }
        }
    }

    void handleSerial_pid_tune(){
        if(Serial.available()){
            char b = Serial.read();
            Serial.flush();  
            if(b=='s'){
                Serial.println("Tuning Starting");
                controller->StartTuning();
            }else if(b=='t'){
                Serial.println("Stopping AutoTune");
                controller->StopTuning();
            }
        }
    }

private:
    //HeaterControllerConfig config;
    HeaterController *controller;
};