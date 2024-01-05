/*#pragma once
#include "../State.hpp"
#include "../BurnInTimer.hpp"
#include <ArduinoSTL.h>
#include <ArduinoComponents.h>
#include <ArduinoJson.h>
#include <SD.h>

using namespace std;
using namespace components;

#define StateFIle      "system_data.txt"
#define TimerFile      "timers.txt"


class Logger{
private:
    vector<timer_data> timer_states;
    system_state* sysState;
    
public:
    void LoadState(){
        File file = SD.open(StateFile);
        StaticJsonDocument<128> doc;

        DeserializationError error = deserializeJson(doc, input);

        if (error) {
            // Serial.print(F("deserializeJson() failed: "));
            // Serial.println(error.f_str());
            return;
        }
        if(this->sysState!=nullptr){
            auto running=doc["Running"].as<bool>();
            auto paused=doc["Paused"].as<bool>();
            this->sysState->CurrentSwitchEnabled=doc["CurrentSwEnabled"].as<bool>();
            this->
        }
        bool Running = doc["Running"]; // true
        bool Paused = doc["Paused"]; // false
        int Temperatue_SP = doc["Temperatue_SP"]; // 85
        bool CurrentSwEnabled = doc["CurrentSwEnabled"]; // true
        int CurrentSetPoint = doc["CurrentSetPoint"]; // 150
    }

};*/