#include "CurrentSensor.hpp"

CurrentSensor::CurrentSensor(PinNumber pin):Component(),
    currentIn(pin),
    fWeight(DEFAULT_FWEIGHT){
    this->current=0;
}

CurrentSensor::CurrentSensor(const CurrentSensorConfig& config)
        :Component(),currentIn(config.Pin),
        fWeight(config.fWeight){
}

double CurrentSensor::ReadCurrent() {
    int value=this->currentIn.read();
    value=map(value,ADC_MIN,ADC_MAX,CURRENT_MIN,CURRENT_MAX);
    this->current+=(((float)value)-this->current)*this->fWeight;
    return this->current;
}

float CurrentSensor::GetCurrent(){
    return this->current;
}