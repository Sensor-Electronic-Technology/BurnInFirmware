#pragma once
#include <ArduinoJson.h>
#include "../constants.h"

typedef struct Configuration{
public:
    virtual void Serialize(JsonObject*)=0;
    virtual void Serialize(JsonObject*,bool)=0;
    virtual void Deserialize(JsonObject)=0;
};

class ControllerConfiguration{
public:
    virtual void Deserialize(JsonDocument doc)=0;
    virtual void Serialize(JsonDocument *doc,bool initialize)=0;
};





typedef struct BurnTimerConfiguration{
	unsigned long time60mASecs=TIME_SECS_60mA;
	unsigned long time120mASecs=TIME_SECS_120mA;
	unsigned long time150mASecs=TIME_SECS_150mA;
}TimingConfiguration;


#pragma region ComponentConfigs

typedef struct CurrentSelectorConfig{
    int pin120mA=PIN_CURRENT_120mA;
    int pin60mA=PIN_CURRENT_60mA;
    int currentPin=PIN_CURRENT;
}CurrentSelectorConfig;








#pragma endregion





