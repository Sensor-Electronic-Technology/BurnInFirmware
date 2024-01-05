#pragma once
#include <ArduinoComponents.h>

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

//typedef components::Function<void(int)> IntCallback;

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


//Message Indexes
#define BurnInCompleteMsg       0
#define NotInRangeMsg           1
#define ResettingSystemMsg      2
#define TestingProbeMsg         3
#define TestCompleteMsg         4
#define SettingTempToMsg        5
#define SetTempToZeroMsg        6
#define TestPausedMsg           7
#define TestResumedMsg          8
#define SetCurrentToMsg         9
#define NoSwitchingMsg          10
#define SystemSettingsRecMsg    11
#define ErrorRecievedMsg        12

#define FimwareInitMsg          13
#define InternalMemMsg          14
#define MemCheckFinishedMsg     15
#define PrintMemMsg             16
#define SettingIOMsg            17
#define TakingMeasMsg           18
#define IOCompleteMsg           19
#define TimerInitMsg            20
#define TimerCompleteMsg        21
#define FirmwareCompleteMsg     22
#define SystemSettingsRecMsg    23
#define HEATER_MODE_TUNING      24
#define HEATER_MODE_NORMAL      25

const char* const message_table[] PROGMEM = {
"[T]{Burn-In Complete.  Heaters Off}\n[T]{Reset before starting next Burn-In}", 
"[T]{Temperatures are not in range}", 
"[T]{Resetting Device}",
"[T]{Testing Probe Contact - Probe on for 1000msec}", 
"[T]{Testing Complete}", 
"[T]{Setting Temperature to ",
"[T]{Setting Temperature to 0}",
"[T]{Device Paused}",
"[T]{Device Resumed}",
"[T]{Setting Current to ",
"[T]{Switching Not Installed On This Station}",
"[T]{Setting Recieved}",
"[T]{Error Recieving Msg}",
"[T]{Starting Firmware Initialization}",
"[T]{Checking internal memory}",
"[T]{Memory check finished}",
"[T]{System State and System Settings:}",
"[T]{Setting up IO}",
"[T]{Taking initial measurements, please wait...}",
"[T]{IO Setup Complete}",
"[T]{Initializing Timers}",
"[T]{Timer Initialization Complete}",
"[T]{Firmware Initialization Complete}",
"[T]{New System Settings Recieved:}",
"[T]{Heater mode changed to Tuning}",
"[T]{Heater mode changed to Normal}"
};

const char* const json_filenames[] PROGMEM = {
    "/hConfigs.txt",
    "/pConfigs.txt"
};

const char* const prefixes[] PROGMEM = {
    "CH",
    "CP",
    "CS"
};

