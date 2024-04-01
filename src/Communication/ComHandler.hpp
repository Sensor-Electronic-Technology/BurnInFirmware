#pragma once
#include <SD.h>
#include <StreamUtils.h>
#include <ArduinoJson.h>
#include "../Heaters/HeaterConfiguration.hpp"
#include "../Probes/ProbeConfiguration.hpp"
#include "../Controller/ControllerConfiguration.hpp"
#include "../Serializable.hpp"
#include "../constants.h"
#include "avr/pgmspace.h"

#define ARDUINOJSON_DEFAULT_NESTING_LIMIT       50

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

    static void MapHeaterResponseCallback(ResponseCallback cbk){
        auto instance=ComHandler::Instance();
        instance->heaterResponseCb=cbk;
    }

    static void MapTestResponseCallback(ResponseCallback cbk){
        auto instance=ComHandler::Instance();
        instance->testResponseCb=cbk;
    }

    static void SendStartResponse(bool success,const __FlashStringHelper* msg){
        auto instance=ComHandler::Instance();
        char buffer[255];
        PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
        strcpy_P(buffer,msgMem);
        instance->InstanceSendStartResponse(success,buffer);
    }

    static void SendStartFromLoad(bool success,const __FlashStringHelper* msg){
        auto instance=ComHandler::Instance();
        char buffer[255];
        PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
        strcpy_P(buffer,msgMem);
        instance->InstanceSendTestStartFromLoad(success,buffer);
    }

    static void SendTestCompleted(const __FlashStringHelper* msg){
        auto instance=ComHandler::Instance();
        char buffer[255];
        PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
        strcpy_P(buffer,msgMem);
        instance->InstanceSendTestCompleted(buffer);
    }

    static void HandleSerial(){
        auto instance=ComHandler::Instance();
        if(instance->serialEventEnabled && instance->serial!=nullptr){
            instance->serialEventEnabled=false;
            instance->serializerDoc.clear();
            auto error=deserializeJson(instance->serialEventDoc,*instance->serial);
            if(error){
                //StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Deserialization Failed.. Reason: \n %s",error.c_str());
                instance->serialEventEnabled=true;
                return;
            }
            instance->MsgPacketDeserialize();
            instance->serialEventEnabled=true;
        }
    }

    template<typename T> 
    static void MsgPacketSerializer(const T& data,PacketType packetType){
        auto instance=ComHandler::Instance();
        Derived_from<T,Serializable>();
        instance->serializerDoc.clear();
        instance->serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
        JsonObject packet=instance->serializerDoc[F("Packet")].to<JsonObject>();
        data.Serialize(&packet,true);
        serializeJson(instance->serializerDoc,*instance->serial);
        instance->serial->println();
        instance->serialEventDoc.clear();
    }

    template <typename T> 
    static void SendRequest(PacketType packetType,const char* request,const T& data) {
        auto instance=ComHandler::Instance();
        instance->InstanceSendRequest(packetType,request,data);
    }

    static void SendMessage(const char* message){
        auto instance=ComHandler::Instance();
        instance->InstanceSendMessage(message);
    }

    static void SendInitMessage(const char* message){
        auto instance=ComHandler::Instance();
        instance->InstanceSendInitMessage(message);
    }

private:
    void MsgPacketDeserialize();
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
    void InstanceSendMessage(const char* message);
    void SendId();
    void ReceiveId();
    void SendVersion();
    void ReceiveVersion();
    void InstanceSendInitMessage(const char* message);
    void InstanceSendStartResponse(bool success,const char* message);

    void InstanceSendTestStartFromLoad(bool success,const char* message);
    void InstanceSendTestCompleted(const char* message);

private:
    static ComHandler* instance;
    JsonDocument serialEventDoc;
    JsonDocument serializerDoc;
    Stream* serial=nullptr;
    bool printInitialized;
    bool fileInitialized;
    bool serialEventEnabled;
    CommandCallback _commandCallback=[](StationCommand){};
    ResponseCallback heaterResponseCb=[](Response){};
    ResponseCallback testResponseCb=[](Response){};
    bool awaitingHeaterResponse=false;
    bool awaitingTestResponse=false;

};