#include "VoltageSensor.hpp"

VoltageSensor::VoltageSensor(PinNumber voltagePin)
    :Component(),voltageIn(voltagePin),
    fWeight(DEFAULT_FWEIGHT) {
    this->voltage=0.0;
}

VoltageSensor::VoltageSensor(const VoltageSensorConfig& config)
    :Component(),
    voltageIn(config.Pin),
    fWeight(config.fWeight),
    configuration(config){
}

VoltageSensor::VoltageSensor():Component(),
    fWeight(DEFAULT_FWEIGHT) {
    this->voltage=0.0;
}

void VoltageSensor::Setup(const VoltageSensorConfig& config){
    this->voltageIn.setAnalogPin(config.Pin);
    this->configuration=config;
    this->fWeight=config.fWeight;
}

double VoltageSensor::ReadVoltage() {
    float value=this->voltageIn.read();
    value=map(value,ADC_MIN,ADC_MAX,VOLTAGE_MIN,VOLTAGE_MIN);
    this->voltage += (((float)value) - this->voltage) * this->fWeight;
    return this->voltage;
}

float VoltageSensor::GetVoltage() {
    return this->voltage;
}