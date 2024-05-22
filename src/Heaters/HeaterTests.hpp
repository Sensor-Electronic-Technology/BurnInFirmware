#pragma once
#include <ArduinoJson.h>
#include "../Communication/ComHandler.hpp"
#include "../Files/FileManager.hpp"
#include "HeaterController.hpp"
#include "HeaterConfiguration.hpp"



 class HeaterTests{
public:
    //HeaterTests(HeaterController *_controller):controller(_controller){   }

    void setup_pid(){
        Serial.begin(38400);
        while(!Serial){}
        ComHandler::SetSerial(&Serial);
        sdInitialized=true;
/*         if(!SD.begin(SS)){
            ComHandler::SendErrorMessage(SystemError::SD_INIT_FAILED);
            sdInitialized=false;
        } */
        //ComHandler::SendSystemMessage(SystemMessage::SD_INIT,MessageType::GENERAL);

        //FileManager::Load(&configuration,PacketType::HEATER_CONFIG);
/*         JsonDocument doc;
        configuration.Serialize(&doc,true);
        serializeJsonPretty(doc,Serial);
        Serial.println("Initializing"); */
        //controller=HeaterController(configuration);
        controller.Setup(configuration);
        controller.Initialize();

/*         heater.SetConfiguration(configuration.heaterConfigs[0]);
        heater.MapTurningComplete([&](HeaterTuneResult result){
            Serial.print("Heater: ");
            Serial.print(result.heaterNumber);
            Serial.print(" Kp: ");
            Serial.print(result.kp);
            Serial.print(" Ki: ");
            Serial.print(result.ki);
            Serial.print(" Kd: ");
            Serial.print(result.kd);
        });
        heater.Initialize(); */

        this->print_menu();
    }

    void print_menu(){
        Serial.println("Press 1 to start AutoTune");
        Serial.println("Press 2 to switch to heating mode");
        Serial.println("Press 3 to toggle heaters");
        Serial.println("Press 4 to start tuning");
        Serial.println("Press 5 to stop tuning");
        Serial.println("Press 6 to save tuning");
        Serial.println("Press 7 to discard tuning");
        Serial.println();
    }

    void loop_pid(){
        //heater.loop();
        controller.loop();
    }

    void handleSerial(){
        if(Serial.available()){
            char b = Serial.read();
            Serial.flush();  
            if(b=='1'){
                controller.SwitchToAutoTune();
                //heater.TurnOn();
            }else if(b=='2'){
                controller.SwitchToHeating();
                //heater.TurnOff();
            }else if(b=='3'){
                controller.ToggleHeaters();
                //heater.StartTuning();
            }else if(b=='4'){
                controller.StartTuning();
                //heater.StopTuning();
            }else if(b=='5'){
                controller.StopTuning();
            }else if(b=='6'){
                controller.SaveTuning();
            }else if(b=='7'){
                controller.DiscardTuning();
            }else if(b=='8'){
                this->print_menu();
            }
        }
    }

private:
    HeaterController controller;
    HeaterControllerConfig configuration;
    Heater heater;

}; 