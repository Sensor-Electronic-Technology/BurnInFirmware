#include "CurrentSensor.hpp"

CurrentSensor::CurrentSensor(PinNumber pin):Component(),
    currentIn(pin),
    fWeight(DEFAULT_FWEIGHT){
    pinMode(this->currentIn,INPUT);
    this->current=0;
}

CurrentSensor::CurrentSensor():Component(),
    fWeight(DEFAULT_FWEIGHT){
    this->current=0;
}

void CurrentSensor::Setup(const CurrentSensorConfig& config){
    this->currentIn=config.Pin;
    this->fWeight=config.fWeight;
    this->current-0;
    pinMode(this->currentIn,INPUT);
}

CurrentSensor::CurrentSensor(const CurrentSensorConfig& config)
        :Component(),currentIn(config.Pin),
        fWeight(config.fWeight){
}

float CurrentSensor::ReadCurrent() {
    int value=analogRead(this->currentIn);
    //Serial.println(value);
    value=map(value,ADC_MIN,ADC_MAX,CURRENT_MIN,CURRENT_MAX);
    //Serial.println(value);
    this->current+=(((float)value)-this->current)*this->fWeight;
    return this->current;
}

float CurrentSensor::GetCurrent(){
    return this->current;
}