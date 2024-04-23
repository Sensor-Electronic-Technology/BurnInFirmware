#pragma one
#include "../constants.h"
#include "../Probes/probe_constants.h"
#include "../Serializable.hpp"


class StartTestFromPacket:public Serializable{
public:
    const char* message;
    const char* testId;
    CurrentValue setCurrent;
    int setTemp;

    virtual void Serialize(JsonObject *packet,bool initialize){
        (*packet)[F("Message")]=this->message;
        (*packet)[F("TestId")]=this->testId;
        (*packet)[F("SetCurrent")]=this->setCurrent;
        (*packet)[F("SetTemperature")]=this->setTemp;
    }
    virtual void Serialize(JsonDocument *doc,bool initialize) override{ }
    virtual void Deserialize(JsonDocument &doc) override{ }
    virtual void Deserialize(JsonObject &packet) override{ }
};