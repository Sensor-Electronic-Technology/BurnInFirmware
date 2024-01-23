#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoComponents.h>
#include <ArxContainer.h>

template <class T> int EEPROM_write(int addr, const T& value) {
    const byte* p = (const byte*)(const void*)&value;
    int newAddr;
    for (newAddr = 0; newAddr < sizeof(value); newAddr++) {
        EEPROM.write(addr++, *p++);
    }
    return newAddr;
}//End write any value/type

template <class T> int EEPROM_read(int addr, T& value) {
    byte* p = (byte*)(void*)&value;
    int newAddr;
    for (newAddr = 0; newAddr < sizeof(value); newAddr++)
        *p++ = EEPROM.read(addr++);
    return newAddr;
}//End read any value/type

inline char StationId[4];
#define ID_ADDR     10

#define read_msg_table(msg) ((const char *)pgm_read_ptr(&(message_table[msg])))
#define read_log_prefix(pre) ((const char *)pgm_read_ptr(&(log_level_prefixes[pre])))
#define read_packet_prefix(pre) ((const char *)pgm_read_ptr(&(prefixes[pre])))
#define read_filename(pType) ((const char *)pgm_read_ptr(&(json_filenames[pType])))
#define read_log_file() ((const char *)pgm_read_ptr(&(log_file)))

//Timer
#define TIMER_PERIOD                   1
#define TIMER_FACTOR                   1000

//LED Control
#define PIN_CURRENT		               2
#define PIN_CURRENT_120mA		       6
#define PIN_CURRENT_60mA               7

#define TIME_SECS_120mA                 72000ul
#define TIME_SECS_60mA                  TIME_SECS_120mA
#define TIME_SECS_150mA                 25200ul


#define TEMP_INTERVAL                   100ul   
#define COM_INTERVAL                    1000ul  //1sec
#define LOG_INTERVAL                    60000ul //1min
#define UPDATE_INTERVAL                 500ul  //500ms
#define DEFAULT_CURRENT                 CurrentValue::c150
#define DEFAULT_FWEIGHT                 0.1

#define ADC_MIN                         0
#define ADC_MAX                         1023

enum StationCommand:uint8_t{
    START=0,
    PAUSE=1,
    TOGGLE_HEAT=2,
    CYCLE_CURRENT=3,
    PROBE_TEST=4,
    CHANGE_MODE_ATUNE=5,
    CHANGE_MODE_NORMAL=6,
    START_TUNE=7,
    STOP_TUNE=8,
    SAVE_ATUNE_RESULT=9,
    RESET=10
};

enum PacketType:uint8_t{
    HEATER_CONFIG=0,
    PROBE_CONFIG=1,
    SYSTEM_CONFIG=2,
    SAVE_STATE=3,
    MESSAGE=4,
    DATA=5,
    COMMAND=6,
    HEATER_RESPONSE=7, //Pc sends save response
    TEST_RESPONSE=8,  //PC sends continue test request
    HEATER_REQUEST=9,  //Pc recieves AutoTuneValues and request save response
    TEST_REQUEST=10,   //Firmware sends continue test request
    ID_RECEIVE=11,        //Send Id to PC
    ID_REQUEST=12      //Set new Id
};

const char* const prefixes[] PROGMEM = {
    "CH",   //0
    "CP",   //1
    "CS",   //2
    "ST",   //3
    "M",    //4
    "D",    //5
    "COM",  //6
    "HRES", //7
    "TRES", //8
    "HREQ", //9
    "TREQ", //10
    "IDREC",  //11
    "IDREQ"   //12
};

const char* const json_filenames[] PROGMEM = {
    "/hConfigs.txt",  //0
    "/pConfigs.txt",  //1
    "/sConfig.txt",   //2
    "/state.txt"      //3
};

enum Response{
    HEATER_SAVE=0,
    HEATER_CANCEL=1,
    TEST_CONTINUE=2,
    TEST_CANCEL=3
};

enum LogLevel:uint8_t{
    ERROR=0,
    CRITICAL_ERROR=1,
    WARNING=2,
    INFO=3
};

const char* const log_level_prefixes[] PROGMEM={
    "ERR:",      //0
    "CRIT_ERR:", //1
    "WARN:"      //2
    "INFO:"      //3
};



typedef components::Function<void(void)> RestartRequiredCallback;
typedef components::Function<void(StationCommand)> CommandCallback;
typedef components::Function<void(Response)> ResponseCallback;

//File indexes
#define HEATER_CONFIG_INDEX     0
#define PROBE_CONFIG_INDEX      1
#define SYSTEM_CONFIG_INDEX     2

enum SystemMessage:uint8_t{
    PID_ATUNE_START_MSG,
    PID_ATUNE_STOP_MSG,
    PID_ATUNE_MODE_ERR_MSG,
    STATION_MODE_ATUNE_MSG,
    STATION_MODE_NORM_MSG,
    STATION_MODE_PROBE_MSG,
    TEST_RUN_MODESW_ERR_MSG,
};

const char* const message_table[] PROGMEM={
    "PID Auto-tune started",
    "PID Auto-tune stopped",
    "Station not in PID Auto-tune mode, switch modes then try again",
    "Station mode switched to PID Autotune",
    "Station mode switched to Normal",
    "Station mode switch to Probe Testing",
    "Modes cannot be switched while a test is running, reset or wait for test to finish",

};

const char* const log_file PROGMEM={"log.txt"};








