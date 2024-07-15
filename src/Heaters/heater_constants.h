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
    unsigned long windowSize=1000;
    void clear(){
        this->heaterNumber=-1;
        this->complete=false;
        this->kp=0;
        this->ki=0;
        this->kd=0;
        this->windowSize=0;
    }

    virtual void Serialize(JsonObject *packet,bool initialize){
        (*packet)[F("HeaterNumber")]=this->heaterNumber;
        (*packet)[F("kp")]=this->kp;
        (*packet)[F("ki")]=this->ki;
        (*packet)[F("kd")]=this->kd;
        (*packet)[F("WindowSize")]=this->windowSize;
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
        heaterTune[F("WindowSize")]=this->windowSize;
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

#define HEATER_DEBUG 		1
#define HEATER_STATE_COUNT 	3
#define TUNE_STATE_COUNT 	3
#define MODE_COUNT          2

/* //NTC-1 Values  10
#define NTC1_A	0.00132264118233491
#define NTC1_B  0.000112787791062004
#define NTC1_C  0.0000012846297515461
//NTC-2 Values
#define NTC2_A	0.00134853900930964
#define NTC2_B  0.000150911950453343
#define NTC2_C  7.97334122501061e-7
//NTC-3 Values
#define NTC3_A	0.00126818340314932
#define NTC3_B  0.00014989501956629
#define NTC3_C  9.00931599413797e-7 */

//Station 9
/* #define NTC1_A	0.0009048314990115
#define NTC1_B  0.000207341005493658
#define NTC1_C  6.96780881482803e-7
//NTC-2 Values
#define NTC2_A	0.000678424348172734
#define NTC2_B  0.00027373400501987
#define NTC2_C  1.82473683435364e-7
//NTC-3 Values
#define NTC3_A	0.00165578046412743
#define NTC3_B  0.0000913528903830797
#define NTC3_C  0.00000108960059707854 */

//Station 4
/* #define NTC1_A	0.00090578537893032
#define NTC1_B  0.000216847312412274
#define NTC1_C  .87102532634484e-7
//NTC-2 Values
#define NTC2_A	0.00105417599623743
#define NTC2_B  0.000189192154765001
#define NTC2_C  7.08523957926431e-7
//NTC-3 Values
#define NTC3_A	0.00109374091173517
#define NTC3_B  0.000192078010835684
#define NTC3_C  6.22634180599287e-7 */

/* //Station 3
#define NTC1_A	0.000903903098729673
#define NTC1_B  0.00020541621545903
#define NTC1_C  7.19934029950399e-7
//NTC-2 Values
#define NTC2_A	0.00115802409151273
#define NTC2_B  0.000181369373377678
#define NTC2_C  6.65572940992084e-7
//NTC-3 Values
#define NTC3_A	0.00121895370275201
#define NTC3_B  0.000169750904102191
#define NTC3_C  7.20352654377522e-7
 */

//Station 2
#define NTC1_A	0.0010403546818096
#define NTC1_B  0.000195407080162592
#define NTC1_C  6.61503783427737e-7
//NTC-2 Values
#define NTC2_A	0.00094001114756494
#define NTC2_B  0.00021125166235281
#define NTC2_C  5.93709983452035e-7
//NTC-3 Values
#define NTC3_A	0.000938545907702812
#define NTC3_B  0.000214775563792224
#define NTC3_C  5.49599295438701e-7


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
#define DEFAULT_WINDOW	                1000
#define DEFAULT_TEMPSP	                85




