#pragma once
#include <SD.h>

#include <ArduinoJson.h>
#include "../Heaters/HeaterConfiguration.hpp"
#include "../Probes/ProbeConfiguration.hpp"
#include "../Controller/ControllerConfiguration.hpp"
#include "../Serializable.hpp"
#include "../constants.h"
#include "SystemMessagePacket.hpp"
#include "avr/pgmspace.h"

#define ARDUINOJSON_DEFAULT_NESTING_LIMIT       50
#define BUFFER_SIZE                             255


class ComHandler{
public:
    static ComHandler* const Instance(){
        if(instance==nullptr){
            instance=new ComHandler;
        }
        return instance;
    }

    static void EnableSerialEvent(){
        auto instance=ComHandler::Instance();
        if(instance->serial!=nullptr){
            instance->serialEventEnabled=true;
            //instance->SendVersion();
            //instance->serial->flush();
        }
    }

    static void SendOutVersion(){
        auto instance=ComHandler::Instance();
        instance->SendVersion();
    }

    static void DisabledSerialEvent(){
        auto instance=ComHandler::Instance();
        if(instance->serial!=nullptr){
            instance->serialEventEnabled=false;
        }
    }

    static void SetSerial(Stream* _serial){
        auto instance=ComHandler::Instance();
        instance->serial=_serial;
    }

    static void MapCommandCallback(CommandCallback cbk){
        auto instance=ComHandler::Instance();
        instance->_commandCallback=cbk;
    }

    static void SendStartResponse(bool success,const __FlashStringHelper* msg){
        auto instance=ComHandler::Instance();
        char buffer[BUFFER_SIZE];
        PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
        strcpy_P(buffer,msgMem);
        instance->InstanceSendStartResponse(success,buffer);
    }

    static void SendSystemMessage(SystemMessage msgIndex,MessageType msgType,...){
        auto instance=ComHandler::Instance();
        char buffer[BUFFER_SIZE];
        SystemMessagePacket msgPacket;
        char format[BUFFER_SIZE];
        strcpy_P(format,read_system_message(msgIndex));
        va_list(args);
        va_start(args,format);
        vsnprintf(buffer,sizeof(buffer),format,args);
        va_end(args);
        msgPacket.message=buffer;
        msgPacket.messageType=msgType;
        ComHandler::MsgPacketSerializer(msgPacket,PacketType::MESSAGE);
    }

    static void SendErrorMessage(SystemError errIndex,...){
        auto instance=ComHandler::Instance();
        char buffer[BUFFER_SIZE];
        SystemMessagePacket msgPacket;
        char format[BUFFER_SIZE];
        strcpy_P(format,read_system_error(errIndex));
        //auto format=read_system_error(errIndex);
        va_list(args);
        va_start(args,format);
        vsnprintf(buffer,sizeof(buffer),format,args);
        va_end(args);
        msgPacket.message=buffer;
        msgPacket.messageType=MessageType::ERROR;
        instance->InstanceMsgPacketSerializer(msgPacket,PacketType::MESSAGE);
    }

    static void SendStartFromLoad(bool success,const __FlashStringHelper* msg){
        auto instance=ComHandler::Instance();
        char buffer[BUFFER_SIZE];
        PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
        strcpy_P(buffer,msgMem);
        instance->InstanceSendTestStartFromLoad(success,buffer);
    }

    static void SendTestCompleted(const __FlashStringHelper* msg){
        auto instance=ComHandler::Instance();
        char buffer[BUFFER_SIZE];
        PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
        strcpy_P(buffer,msgMem);
        instance->InstanceSendTestCompleted(buffer);
    }

    static void HandleSerial(){
        auto instance=ComHandler::Instance();
        JsonDocument serialEventDoc;
        if(instance->serialEventEnabled && instance->serial!=nullptr){
            instance->serialEventEnabled=false;
            auto error=deserializeJson(serialEventDoc,*instance->serial);
            if(error){
                Serial.print(F("Deserialization Failed.. Reason: \n"));
                Serial.println(error.c_str());
                //StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Deserialization Failed.. Reason: \n %s",error.c_str());
                instance->serialEventEnabled=true;
                return;
            }
            instance->MsgPacketDeserialize(serialEventDoc);
            instance->serialEventEnabled=true;
        }
    }

    template<typename T> 
    static void MsgPacketSerializer(const T& data,PacketType packetType){
        JsonDocument serializerDoc;
        auto instance=ComHandler::Instance();
        Derived_from<T,Serializable>();
        serializerDoc.clear();
        char packetStr[BUFFER_SIZE];
        strcpy_P(packetStr,read_packet_prefix(packetType));
        serializerDoc[F("Prefix")]=packetStr;
        JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
        data.Serialize(&packet,true);
        serializeJson(serializerDoc,*instance->serial);
        instance->serial->println();
    }

private:
    void MsgPacketDeserialize(JsonDocument& serialEventDoc);
    void ResponseDeserializer(JsonDocument& doc);
    /**
     * Serializes objects derived from ControllerConfiguration.
     * 
     * @tparam T Type derived from Controller Configuration
     * @param doc json document to be configured
     * @param data data derived from ControllerConfiguration to be serialized
     * @param packetType packet type for prefix PGP array
     * @param initialze delares whether the function needs to initialize the json objects
     * @return void
     */
    template <typename T> 
    void InstanceMsgPacketSerializer(const T& data,PacketType packetType);
    template <typename T> 
    void InstanceSendRequest(PacketType packetType,const char* request,const T& data);
    // void InstanceSendMessage(const char* message);
    void SendId();
    void ReceiveId(const JsonDocument& serialEventDoc);
    void SendVersion();
    void ReceiveVersion(const JsonDocument& serialEventDoc);
    void InstanceSendStartResponse(bool success,const char* message);
    void InstanceSendTestStartFromLoad(bool success,const char* message);
    void InstanceSendTestCompleted(const char* message);


private:
    static ComHandler* instance;
    
    Stream* serial=nullptr;
    bool serialEventEnabled;
    CommandCallback _commandCallback=[](StationCommand){};
};