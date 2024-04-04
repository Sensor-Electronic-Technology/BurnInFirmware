#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include "../Serializable.hpp"


#define read_msg_table(msg) ((const char *)pgm_read_ptr(&(message_table[msg])))

typedef components::Function<void(void)> TransitionActionHandler;

#define HEATER_COUNT                    3
#define TEMP_INTERVAL                   100ul 
#define DEFAULT_TEMP_DEV                0.1

struct HeaterTuneResult:Serializable{
    int heaterNumber=-1;
    bool complete=false;
    float kp=0,ki=0,kd=0;
    void clear(){
        this->heaterNumber=-1;
        this->complete=false;
        this->kp=0;
        this->ki=0;
        this->kd=0;
    }

    virtual void Serialize(JsonObject *packet,bool initialize){
        (*packet)[F("HeaterNumber")]=this->heaterNumber;
        (*packet)[F("kp")]=this->kp;
        (*packet)[F("ki")]=this->ki;
        (*packet)[F("kd")]=this->kd;
    }

    virtual void Serialize(JsonDocument *doc,bool initialize)override{
        JsonObject heaterTune;
        if(initialize){
            heaterTune=(*doc)[F("HeaterTune")].to<JsonObject>();
        }else{
            heaterTune=(*doc)[F("HeaterTune")].as<JsonObject>();
        }
        heaterTune[F("HeaterNumber")]=this->heaterNumber;
        heaterTune[F("kp")]=this->kp;
        heaterTune[F("ki")]=this->ki;
        heaterTune[F("kd")]=this->kd;
    }
    virtual void Deserialize(JsonDocument &doc)  override{   }
    virtual void Deserialize(JsonObject &packet) override{   }
};

enum TuneState:uint8_t{
    TUNE_IDLE,      //Waiting for start command
    TUNE_RUNNING,   //Tuning in progress
    TUNE_COMPLETE,  //Tuning complete, wating for save command
};

enum HeaterState:uint8_t{
    HEATER_OFF=0, //Heater off
    HEATER_WARMUP=1, //Warmup tempOkay=false;
    HEATER_ON=2, //PID ON and warmup complete

};

enum HeaterTransition:uint8_t{
    HEATER_OFF_TO_WARMUP=0,  //Start Warmup
    HEATER_WARMUP_TO_ON=1,  //TempOkay=true
    HEATER_ON_TO_OFF=2,    //Turn Off
    HEATER_WARMUP_TO_OFF=3 //Turn Off
};

enum HeaterTrigger:uint8_t{
    START_HEATERS,
    TEMP_REACHED,
    STOP_HEATERS
};

enum TuneTrigger:uint8_t{
    TUNE_START,     //Start Tuning
    TUNE_FINISHED,  //Tuning Complete
    TUNE_STOP,      //Stop Tuning->cancel before complete
    TUNE_SAVED,     //Save Tuning Results
    TUNE_CANCELED   //Discard Tuning Results
};

enum TuneTransition:uint8_t{
    TUNE_IDLE_TO_RUNNING=0,  //Start Tuning
    TUNE_RUNNING_TO_IDLE=1,  //cancel tuning
    TUNE_RUNNING_TO_COMPLETE=2,  //Tuning Complete
    TUNE_COMPLETE_TO_IDLE=3,  //Save or discard tune results
    TUNE_EXIT_MODE=4
};

enum HeaterMode:uint8_t{
    HEATING=0,      //Normal Heater Mode
    ATUNE=1     //AutoTune Mode
};

enum ModeTrigger:uint8_t{
    ATUNE_START,
    HEATING_START
};

enum ModeTransition:uint8_t{
    MODE_HEATING_TO_ATUNE=0,  //Start AutoTune,
    MODE_ATUNE_TO_HEATING=1  //Stop AutoTune, Start Heating
};

union HeaterStateSelector{
    HeaterState    pidState;
    TuneState   tuneState;
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



//Temperature Constants
#define KELVIN_RT                       273.15
#define R_REF                           1000

//PID Defaults
#define KP_DEFAULT		                2
#define KI_DEFAULT		                5
#define KD_DEFAULT		                1
#define DEFAULT_WINDOW	                250
#define DEFAULT_TEMPSP	                40




