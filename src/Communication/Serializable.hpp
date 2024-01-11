#pragma once
#include <ArduinoJson.h>
#include "../constants.h"
#include <string.h>

using namespace std;

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
class Serializable{
public:
    virtual void Deserialize(JsonDocument &doc)=0;
    virtual void Deserialize(JsonObject &packet)=0;
    virtual void Serialize(JsonDocument *doc,bool initialize)=0;
    virtual void Serialize(JsonObject *packet,bool initialize)=0;

};
























