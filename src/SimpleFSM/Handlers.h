#pragma once
#include "../ArduinoComponents/src/ArduinoComponents/Helpers/Function.h"

typedef components::Function<void(void)> CallbackFunction;
typedef components::Function<bool(void)> GuardCondition;