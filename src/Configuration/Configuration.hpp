#pragma once
#include <ArduinoJson.h>
#include "../constants.h"

/**
 * @brief Compile time check if class is derived from another
 * 
 * @tparam T Class to test if derived from B
 * @tparam B Base class to check against
 */
template<class T, class B> struct Derived_from {
    static void constraints(T* p) { B* pb = p; }
    Derived_from() { void(*p)(T*) = constraints; }
};

template<class T1, class T2> struct Can_copy {
    static void constraints(T1 a, T2 b) { T2 c = a; b = a; }
    Can_copy() { void(*p)(T1,T2) = constraints; }
};

template<class T1, class T2 = T1> struct Can_compare {
    static void constraints(T1 a, T2 b) { a==b; a!=b; a<b; }
    Can_compare() { void(*p)(T1,T2) = constraints; }
};


/**
 * @brief Abstrast class for ControllerConfigurations
 * Defines virtual serializion functions
 */
class ControllerConfiguration{
public:
    virtual void Deserialize(JsonDocument doc)=0;
    virtual void Serialize(JsonDocument *doc,bool initialize)=0;
    virtual void Serialize(JsonObject *packet,bool initialize)=0;
};

enum ConfigType:uint8_t{
    HEATER_CONFIG=0,
    PROBE_CONFIG=1,
    SYSTEM_CONFIG=2
};

/**
 * Serializes objects derived from ControllerConfiguration.
 * 
 * @tparam T Type derived from Controller Configuration
 * @param doc json document to be configured
 * @param data data derived from ControllerConfiguration to be serialized
 * @param configType config type for prefix array index
 * @param initialze delares whether the function needs to initialize the json objects
 * @return void
 */
template <class T> void MsgPacketSerialize(JsonDocument *doc,const T& data,ConfigType configType,bool initialize) {
    (*doc)["Prefix"]=prefixes[configType];
    Derived_from<T,ControllerConfiguration>();
    auto packet=(*doc)["Packet"].to<JsonObject>();
    data.Serialize(&packet,initialize);
}

typedef struct BurnTimerConfiguration{
	unsigned long time60mASecs=TIME_SECS_60mA;
	unsigned long time120mASecs=TIME_SECS_120mA;
	unsigned long time150mASecs=TIME_SECS_150mA;
}TimingConfiguration;


















