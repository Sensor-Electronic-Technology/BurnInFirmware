#pragma once
#include "../constants.h"
#include "../Serializable.hpp"

struct SystemMessagePacket:Serializable{
    MessageType messageType;
    const char* message;

    virtual void Serialize(JsonObject *packet,bool initialize){
        (*packet)[F("MessageType")]=this->messageType;
        (*packet)[F("Message")]=this->message;
    }
    virtual void Serialize(JsonDocument *doc,bool initialize) override{ }
    virtual void Deserialize(JsonDocument &doc) override{ }
    virtual void Deserialize(JsonObject &packet) override{ }
};

