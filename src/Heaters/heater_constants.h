#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include "../Serializable.hpp"


#define read_msg_table(msg) ((const char *)pgm_read_ptr(&(message_table[msg])))

typedef components::Function<void(void)> TransitionActionHandler;

#define HEATER_COUNT                    3
#define TEMP_INTERVAL                   100ul 
#define DEFAULT_TEMP_DEV                0.1
#define MAX_SETPOINT                    100


enum HeatState {
	On,
	Off
};

typedef struct HeaterResult{
	float temperature=0;
	bool state=false;
	bool tempOkay=false;

	HeaterResult operator=(const HeaterResult& rhs){
		this->temperature=rhs.temperature;
		this->state=rhs.state;
		this->tempOkay=rhs.tempOkay;
		return *this;
	}
}HeaterResult;

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

enum HeaterMode:uint8_t{
    HEATING=0,      //Normal Heater Mode
    ATUNE=1     //AutoTune Mode
};

enum ModeTrigger:uint8_t{
    ATUNE_START,
    HEATING_START
};





typedef components::Function<void(HeaterTuneResult)> TuningCompleteCallback;

#define HEATER_DEBUG 		0
#define HEATER_STATE_COUNT 	3
#define TUNE_STATE_COUNT 	3
#define MODE_COUNT          2

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
#define DEFAULT_TEMPSP	                85




