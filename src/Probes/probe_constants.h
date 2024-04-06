#pragma once
#include <Arduino.h>

enum CurrentValue:uint8_t{
    c150=150,
    c120=120,
    c060=60
};

#define PROBE_COUNT                     6
#define PROBE_READINTERVAL              100ul
#define PROBE_TESTTIME                 5000

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

#define VOLTAGE_MAX                     108
#define VOLTAGE_MIN                     0

#define CURRENT_MAX                     250
#define CURRENT_MIN                     0