#include "VoltageSensor.hpp"

VoltageSensor::VoltageSensor(PinNumber voltagePin)
    :Component(),voltageIn(voltagePin),
    fWeight(DEFAULT_FWEIGHT) {
        pinMode(this->voltageIn,INPUT);
    this->voltage=0.0;
}

VoltageSensor::VoltageSensor(const VoltageSensorConfig& config)
    :Component(),
    voltageIn(config.Pin),
    fWeight(config.fWeight){
    pinMode(this->voltageIn,INPUT);
}

VoltageSensor::VoltageSensor():Component(),
    fWeight(DEFAULT_FWEIGHT) {
    this->voltage=0.0;
}

void VoltageSensor::Setup(const VoltageSensorConfig& config){
    this->fWeight=config.fWeight;
    this->voltageIn=config.Pin;
    pinMode(this->voltageIn,INPUT);
}

float VoltageSensor::ReadVoltage() {
    float value=analogRead(this->voltageIn);
    value=map(value,ADC_MIN,ADC_MAX,VOLTAGE_MIN,VOLTAGE_MAX);
    this->voltage += (((float)value) - this->voltage) * this->fWeight;
    return this->voltage;
}

float VoltageSensor::GetVoltage() {
    return this->voltage;
}