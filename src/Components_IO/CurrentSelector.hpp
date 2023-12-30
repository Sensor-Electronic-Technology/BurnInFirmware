#pragma once
#include <ArduinoComponents.h>
#include "../Configuration/Configuration.hpp"
#include "../constants.h"

using namespace components;

class CurrentSelector:public Component{
public:

    CurrentSelector(PinNumber out,PinNumber c1,PinNumber c2)
        :Component(),
        currentPin120(c1),
        currentPin060(c2),
        currentOutput(out),
        setCurrent(DEFAULT_CURRENT){
        this->currentOutput.low();
        this->currentPin120.low();
        this->currentPin060.low();
    }

    CurrentSelector(const CurrentSelectorConfig& config,bool switchEnabled=true,CurrentValue current=DEFAULT_CURRENT)
            :Component(),currentPin120(config.pin120mA),
            currentPin060(config.pin60mA),
            currentOutput(config.currentPin),
            enabled(switchEnabled),
            setCurrent(current),
            configuration(config){
        this->currentOutput.low();
        this->currentPin120.low();
        this->currentPin060.low();
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
                this->currentPin120.low();
                this->currentPin060.low();
                this->currentOutput.high();
                return;
            }
            case CurrentValue::c120: {
                this->currentPin120.high();
                this->currentPin060.low();
                this->currentOutput.high();
                return;
            }
            case CurrentValue::c060: {
                this->currentPin120.low();
                this->currentPin060.high();
                this->currentOutput.high();
                return;
            }
        }
    }

    void TurnOff(){
        this->currentOutput.low();
    }
    
private:
    void privateLoop(){ }

    CurrentSelectorConfig configuration;
    DigitalOutput currentOutput;
    DigitalOutput currentPin120;
    DigitalOutput currentPin060;
    CurrentValue setCurrent;
    bool enabled;
};