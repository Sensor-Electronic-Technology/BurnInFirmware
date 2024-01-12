#pragma once
#include <Arduino.h>
#include <ArduinoComponents.h>
#include <ArxContainer.h>

#define read_msg_table(msg) ((const char *)pgm_read_ptr(&(message_table[msg])))
#define read_log_prefix(pre) ((const char *)pgm_read_ptr(&(log_level_prefixes[pre])))
#define read_packet_prefix(pre) ((const char *)pgm_read_ptr(&(prefixes[pre])))
#define read_filename(pType) ((const char *)pgm_read_ptr(&(json_filenames[pType])))
#define read_log_file() ((const char *)pgm_read_ptr(&(log_file)))

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

enum CurrentValue:int{
    c150=150,
    c120=120,
    c060=60
};

enum StationCommand:uint8_t{
    START,
    PAUSE,
    TOGGLE_HEAT,
    SWITCH_CURRENT,
    PROBE_TEST,
    UPDATE_CONFIG,
    START_TUNE,
};

enum HeaterMode:uint8_t{
    PID_RUN=0,
	ATUNE_RUN=1
};

//typedef components::Function<void(int)> IntCallback;
typedef components::Function<void(void)> RestartRequiredCallback;
typedef components::Function<void(StationCommand)> CommandCallback;
typedef void (*CommandHandlerCallback)(StationCommand);

//Timer
#define TIMER_PERIOD                   1
#define TIMER_FACTOR                   1000

//LED Control
#define PIN_CURRENT		               2
#define PIN_CURRENT_120mA		       6
#define PIN_CURRENT_60mA               7

//Heaters
#define PIN_HEATER1_HEATER		       3
#define PIN_HEATER2_HEATER		       4
#define PIN_HEATER3_HEATER		       5

//Temp Sensors
#define PIN_HEATER1_TEMP		       A6
#define PIN_HEATER2_TEMP		       A7
#define PIN_HEATER3_TEMP		       A8


#define PIN_PROBE1_VOLT			       A0
#define PIN_PROBE2_VOLT			       A1
#define PIN_PROBE3_VOLT			       A2
#define PIN_PROBE4_VOLT			       A3
#define PIN_PROBE5_VOLT			       A4
#define PIN_PROBE6_VOLT			       A5

#define PIN_PROBE1_CURRENT             A9
#define PIN_PROBE2_CURRENT             A10
#define PIN_PROBE3_CURRENT             A11
#define PIN_PROBE4_CURRENT             A12
#define PIN_PROBE5_CURRENT             A13
#define PIN_PROBE6_CURRENT             A14

#define TIME_SECS_120mA                 72000ul
#define TIME_SECS_60mA                  TIME_SECS_120mA
#define TIME_SECS_150mA                 25200ul

#define PROBE_COUNT                     6
#define HEATER_COUNT                    3
#define PROBE_READINTERVAL              100ul
#define TEMP_INTERVAL                   100ul 
#define DEFAULT_TEMP_DEV                0.1
#define DEFAULT_CURRENT                 CurrentValue::c150
#define DEFAULT_FWEIGHT                 0.1

#define KELVIN_RT                       273.15
#define R_REF                           1000

#define ADC_MIN                         0
#define ADC_MAX                         1023

#define VOLTAGE_MAX                     108
#define VOLTAGE_MIN                     0

#define CURRENT_MAX                     250
#define CURRENT_MIN                     0

//PID Defaults
#define KP_DEFAULT		                2
#define KI_DEFAULT		                5
#define KD_DEFAULT		                1
#define DEFAULT_WINDOW	                250
#define DEFAULT_TEMPSP	                40

//File indexes
#define HEATER_CONFIG_INDEX     0
#define PROBE_CONFIG_INDEX      1
#define SYSTEM_CONFIG_INDEX     2

enum PacketType:uint8_t{
    HEATER_CONFIG=0,
    PROBE_CONFIG=1,
    SYSTEM_CONFIG=2,
    MESSAGE=3,
    DATA=4,
    COMMAND=5
};

enum LogLevel:uint8_t{
    ERROR=0,
    CRITICAL_ERROR=1,
    WARNING=2,
    INFO=3
};

enum SystemMessage:uint8_t{
    BURNING_FINISHED=0,
    TEMP_OUT_OF_RANGE=1,
    SYSTEM_RESETTING=2,
    STARTING_PROBE_TEST=3,
    TEMP_SET_TO=4,
    TEST_PAUSED=5,
    TEST_RESUMED=6,
    CURRENT_SET_TO=7,
    SWITCH_DISABLED=8,
    CONFIGS_RECIEVED=9,
    CONFIGS_RECIEVED_ERROR=10,
    FIRMWARE_INIT_MESSAGE=11,
    LOADING_CONFIG_FILES=12,
    LOADING_CONFIG_FILES_DONE=13,
    CHECKING_RUNNING_TEST=14
};

const char* const message_table[] PROGMEM={
    "Burn-In Complete.  Heaters Off}\nReset before starting next Burn-In",
    "Temperatures not in range",
    "Resetting Device",
    "Probe Test Started",
    "Setting Temperature to %d",
    "Test Paused",
    "Test Resumed",
    "Setting Current to %d",
    "Switching disabled on this station",
    "Setting Recieved",
    "Error recieving configuration",
    "Starting Firmware Initialization",
    "Loading configurations",
    "Loading completed",
    "Checking for running test"
};

const char* const log_file PROGMEM={"log.txt"};

const char* const log_level_prefixes[] PROGMEM={
    "ERR:",
    "CRIT_ERR:",
    "WARN:"
    "INFO:"
};


const char* const filenames[] PROGMEM = {
    "/hConfigs.txt",
    "/pConfigs.txt",
    "/sConfig.txt"
};

const char* const prefixes[] PROGMEM = {
    "CH",
    "CP",
    "CS",
    "M",
    "D"
};


/*const static arx::map<MessageType,const char*> msgMap {
    {MessageType::BURNING_FINISHED,},
    {MessageType::TEMP_OUT_OF_RANGE,},
    {MessageType::SYSTEM_RESETTING,},
    {MessageType::STARTING_PROBE_TEST,},
    {MessageType::TEMP_SET_TO,},
    {MessageType::TEST_PAUSED,},
    {MessageType::TEST_RESUMED,},
    {MessageType::CURRENT_SET_TO,},
    {MessageType::SWITCH_DISABLED,},
    {MessageType::CONFIGS_RECIEVED,},
    {MessageType::CONFIGS_RECIEVED_ERROR,},
    {MessageType::FIRMWARE_INIT_MESSAGE,},
    {MessageType::LOADING_CONFIG_FILES,},
    {MessageType::LOADING_CONFIG_FILES_DONE,},
    {MessageType::CHECKING_RUNNING_TEST,}
    
};*/

