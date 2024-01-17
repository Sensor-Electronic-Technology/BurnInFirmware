#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>

struct HeaterTuneResult{
    int heaterNumber=-1;
    bool complete=false;
    double kp=0,ki=0,kd=0;
    void clear(){
        this->heaterNumber=-1;
        this->complete=false;
        this->kp=0;
        this->ki=0;
        this->kd=0;
    }
};

enum HeaterMode:uint8_t{
    PID_RUN=0,
	ATUNE_RUN=1
};



typedef components::Function<void(HeaterTuneResult)> TuningCompleteCallback;

//NTC-1 Values
#define NTC1_A	1.159e-3f
#define NTC1_B  1.429e-4f
#define NTC1_C  1.118e-6f
//NTC-2 Values
#define NTC2_A	1.173e-3f
#define NTC2_B  1.736e-4f
#define NTC2_C  7.354e-7f
//NTC-3 Values
#define NTC3_A	1.200e-3f
#define NTC3_B  1.604e-4f
#define NTC3_C  8.502e-7f

//Heaters
#define PIN_HEATER1_HEATER		       3
#define PIN_HEATER2_HEATER		       4
#define PIN_HEATER3_HEATER		       5

//Temp Sensors
#define PIN_HEATER1_TEMP		       A6
#define PIN_HEATER2_TEMP		       A7
#define PIN_HEATER3_TEMP		       A8

#define HEATER_COUNT                    3
#define TEMP_INTERVAL                   100ul 
#define DEFAULT_TEMP_DEV                0.1

//Temperature Constants
#define KELVIN_RT                       273.15
#define R_REF                           1000

//PID Defaults
#define KP_DEFAULT		                2
#define KI_DEFAULT		                5
#define KD_DEFAULT		                1
#define DEFAULT_WINDOW	                250
#define DEFAULT_TEMPSP	                40

