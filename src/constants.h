#pragma once

enum CurrentValue:int{
    c150=150,
    c120=120,
    c060=60
};

//LED Control
#define PIN_CURRENT		              2
#define PIN_CURRENT_120mA		      6
#define PIN_CURRENT_60mA              7

//Heaters
#define PIN_HEATER1_HEATER		      3
#define PIN_HEATER2_HEATER		      4
#define PIN_HEATER3_HEATER		      5

//Temp Sensors
#define PIN_HEATER1_TEMP		      A6
#define PIN_HEATER2_TEMP		      A7
#define PIN_HEATER3_TEMP		      A8


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

#define TIME_SECS_120mA           72000ul
#define TIME_SECS_60mA            TIME_SECS_120mA
#define TIME_SECS_150mA           25200ul

#define PROBE_COUNT         6
#define HEATER_COUNT        3
#define DEFAULT_CURRENT     CurrentValue::c150
#define DEFAULT_FWEIGHT     0.1

#define KELVIN_RT           273.15
#define R_REF               1000

#define ADC_MIN             0
#define ADC_MAX             1023

#define VOLTAGE_MAX         108
#define VOLTAGE_MIN         0

#define CURRENT_MAX          250
#define CURRENT_MIN          0

//PID Defaults
#define KP_DEFAULT		    2
#define KI_DEFAULT		    5
#define KD_DEFAULT		    1
#define DEFAULT_WINDOW	    500
#define DEFAULT_TEMPSP	    85
