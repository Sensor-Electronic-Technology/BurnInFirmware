#pragma once
#include <ArduinoComponents.h>
#include "ProbeConfiguration.hpp"
#include "probe_constants.h"

using namespace components;

class CurrentSelector:public Component{
public:
    CurrentSelector(const CurrentSelectorConfig& config,CurrentValue current=DEFAULT_CURRENT)
            :Component(),currentPin120(config.pin120mA),
            currentPin060(config.pin60mA),
            currentOutput(config.currentPin),
            switchEnabled(config.switchEnabled),
            setCurrent(current){
            pinMode(this->currentOutput,OUTPUT);
            pinMode(this->currentPin060,OUTPUT);
            pinMode(this->currentPin120,OUTPUT);
            digitalWrite(this->currentOutput,LOW);
            digitalWrite(this->currentPin060,LOW);
            digitalWrite(this->currentPin120,LOW);
    }

    CurrentSelector():Component(){
        digitalWrite(this->currentOutput,LOW);
        digitalWrite(this->currentPin060,LOW);
        digitalWrite(this->currentPin120,LOW);
    }

    void Setup(const CurrentSelectorConfig& config){
        this->currentPin120=config.pin120mA;
        this->currentPin060=config.pin60mA;
        this->currentOutput=config.currentPin;

        this->setCurrent=config.SetCurrent;
        this->switchEnabled=config.switchEnabled;
        pinMode(this->currentOutput,OUTPUT);
        pinMode(this->currentPin060,OUTPUT);
        pinMode(this->currentPin120,OUTPUT);
        digitalWrite(this->currentOutput,LOW);
        digitalWrite(this->currentPin060,LOW);
        digitalWrite(this->currentPin120,LOW);
    }

    void SetCurrent(CurrentValue current){
        this->setCurrent=current;
    }

    CurrentValue GetSetCurrent(){
        return this->setCurrent;
    }

    CurrentValue CycleCurrent(){
        switch(this->setCurrent){
            case CurrentValue::c150: {
                this->setCurrent=CurrentValue::c120;
                break;
            }
            case CurrentValue::c120: {
                this->setCurrent=CurrentValue::c060;
                break;
            }
            case CurrentValue::c060: {
                this->setCurrent=CurrentValue::c150;
                break;
            }
        }
         return this->setCurrent;
    }

    void TurnOn(){
        switch(this->setCurrent){
            case CurrentValue::c150: {
                digitalWrite(this->currentPin120,LOW);
                digitalWrite(this->currentPin060,LOW);
                digitalWrite(this->currentOutput,HIGH);
                return;
            }
            case CurrentValue::c120: {
                digitalWrite(this->currentPin120,HIGH);
                digitalWrite(this->currentPin060,LOW);
                digitalWrite(this->currentOutput,HIGH);
                return;
            }
            case CurrentValue::c060: {
                digitalWrite(this->currentPin120,LOW);
                digitalWrite(this->currentPin060,HIGH);
                digitalWrite(this->currentOutput,HIGH);
                return;
            }
        }
    }

    void TurnOff(){
        digitalWrite(this->currentOutput,LOW);
    }
    
private:
    void privateLoop(){ }
    uint8_t currentOutput,currentPin120,currentPin060;
    CurrentValue setCurrent;
    bool switchEnabled;
};