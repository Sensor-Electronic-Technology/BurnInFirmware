#include "TemperatureSensor.hpp"

TemperatureSensor::TemperatureSensor(PinNumber pin,float a,float b,float c) 
    :Component(),input(pin),temperature(0),
    aValue(a),bValue(b),cValue(c),fWeight(DEFAULT_FWEIGHT) {   }

TemperatureSensor::TemperatureSensor(const NtcConfig& config)
    :Component(),
    input(config.Pin),
    configuration(config),
    aValue(config.aCoeff),
    bValue(config.bCoeff),
    cValue(config.cCoeff),
    fWeight(config.fWeight){ /***/ }

double TemperatureSensor::Read(){
    double aValue = this->input.read();
    double ntc_res=R_REF/(ADC_MAX/aValue-1);
    //Steinhart and Hart Equation 1/A+Bln(R)+C[ln(R)]^3  
    double temp=(1/(this->aValue+(this->bValue*log(ntc_res))+(this->cValue*pow(log(ntc_res),3))))-KELVIN_RT;
    this->temperature+=(temp-this->temperature)*fWeight;
    return this->temperature;
}

float TemperatureSensor::GetTemperature() {
    return this->temperature;
}

void TemperatureSensor::UpdateConfig(const NtcConfig& config){
    //if pin is changed the controller will need to be reset
    this->configuration=config;
    this->aValue=config.aCoeff;
    this->bValue=config.bCoeff;
    this->cValue=config.cCoeff;
    this->fWeight=config.fWeight;
}