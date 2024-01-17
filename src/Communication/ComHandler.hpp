#pragma once
#include <SD.h>
#include <StreamUtils.h>
#include <ArduinoJson.h>
#include "../Heaters/HeaterConfiguration.hpp"
#include "../Probes/ProbeConfiguration.hpp"
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
            //instance->serial->flush();
        }
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

    static void HandleSerial(){
        auto instance=ComHandler::Instance();
        if(instance->serialEventEnabled && instance->serial!=nullptr){
            instance->serialEventEnabled=false;
            instance->serializerDoc.clear();
            auto error=deserializeJson(instance->serialEventDoc,*instance->serial);
            if(error){
                //StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,"Deserialization Failed.. Reason: \n %s",error.c_str());
                instance->serialEventEnabled=true;
                return;
            }
            instance->MsgPacketDeserialize();
            instance->serialEventEnabled=true;

        }
    }

    void MsgPacketDeserialize() {
        JsonDocument doc(instance->serialEventDoc);
        const char* prefix= doc[F("Prefix")].as<const char*>();
        //StationLogger::Log(LogLevel::INFO,true,false,"Input Prefix: %s",prefix);
        bool found=false;
        PacketType packetType;    
        for(uint8_t i=0;i<7;i++){
            const char* target=read_packet_prefix(i);
            if(strcmp(prefix,target)==0){
                found=true;
                packetType=(PacketType)i;
                break;
            }
        }
        if(found){
            //StationLogger::Log(LogLevel::INFO,true,false,"Prefix found!!!!!!!! Prefix: %s",prefix);
            switch(packetType){
                case PacketType::PROBE_CONFIG:{
                    auto packet=doc[F("Packet")].as<JsonObject>();
                    //StationLogger::Log(LogLevel::INFO,true,false,"Is ProbeConfig, Deserializing..");
                    ProbeControllerConfig config;
                    config.Deserialize(packet);
                    //StationLogger::Log(LogLevel::INFO,true,false,"Sending back serialized using MsgPack method");
                    this->InstanceMsgPacketSerializer(config,PacketType::PROBE_CONFIG);
                    break;
                }
                case PacketType::HEATER_CONFIG:{
                    auto packet=doc[F("Packet")].as<JsonObject>();
                    //StationLogger::Log(LogLevel::INFO,true,false,"Is HeaterConfig, Deserializing..");
                    HeaterControllerConfig config;
                    config.Deserialize(packet);
                    //StationLogger::Log(LogLevel::INFO,true,false,"Sending back serialized using MsgPack method");
                    this->InstanceMsgPacketSerializer(config,PacketType::PROBE_CONFIG);
                    break;
                }
                case PacketType::SYSTEM_CONFIG:{
                    auto packet=doc[F("Packet")].as<JsonObject>();
                    //instance->SendMessage("INFO:Is SystemConfig, Deserializing..");
                    HeaterControllerConfig config;
                    config.Deserialize(packet);
                    //instance->SendMessage("INFO:Sending back serialized using MsgPack method");
                    this->InstanceMsgPacketSerializer(config,PacketType::PROBE_CONFIG);
                    break;
                }
                case PacketType::COMMAND:{
                    auto packet=doc[F("Packet")].as<JsonObject>();
                    StationCommand command=(StationCommand)doc[F("Packet")][F("Command")];
                    instance->_commandCallback(command);
                    break;
                }
                case PacketType::HEATER_RESPONSE:{
                    //StationLogger::Log(LogLevel::ERROR,true,false,"Reuest type not implemented");
                }
                case PacketType::TEST_RESPONSE:{
                    //StationLogger::Log(LogLevel::ERROR,true,false,"Reuest type not implemented");
                    this->ResponseDeserializer(doc);
                    break;
                }
                default:{
                    //StationLogger::Log(LogLevel::INFO,true,false,"Invalid prefix.. Prefix: %s",prefix);
                    break;
                }
            }
        }else{
            //StationLogger::Log(LogLevel::ERROR,true,false,"Prefix not found..");

        }
    }

    void ResponseDeserializer(JsonDocument& doc){
        auto packet=doc[F("Packet")].as<JsonObject>();
        Response response=(Response)packet[F("Response")];
        switch(response){
            case Response::HEATER_SAVE:
            case Response::HEATER_CANCEL:{
                this->heaterResponseCb(response);
                break;
            }
            case Response::TEST_CONTINUE:
            case Response::TEST_CANCEL:{
                this->testResponseCb(response);
                break;
            }
        }

    }

    template<typename T> static void MsgPacketSerializer(const T& data,PacketType packetType){
        auto instance=ComHandler::Instance();
        instance->InstanceMsgPacketSerializer(data,packetType);
    }

    template <typename T> 
    static void SendRequest(PacketType packetType,const char* request,const T& data) {
        auto instance=ComHandler::Instance();
        Derived_from<T,Serializable>();
        instance->serializerDoc.clear();
        instance->serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
        instance->serializerDoc[F("RequestText")]=request;
        auto packet=instance->serializerDoc[F("Packet")].to<JsonObject>();
        data.Serialize(&packet,true);
        serializeJson(instance->serializerDoc,*instance->serial);
        instance->serial->println();
    }

    static void SendMessage(const char* message){
        auto instance=ComHandler::Instance();
        instance->serializerDoc.clear();
        instance->serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::MESSAGE);
        auto packetJson=instance->serializerDoc[F("Packet")].to<JsonObject>();
        packetJson[F("Message")]=message;
        serializeJson(instance->serializerDoc,*instance->serial);
        instance->serial->println();
    }

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
    void InstanceMsgPacketSerializer(const T& data,PacketType packetType) {
        Derived_from<T,Serializable>();
        this->serializerDoc.clear();
        this->serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
        auto packet=this->serializerDoc[F("Packet")].to<JsonObject>();
        data.Serialize(&packet,true);
        serializeJson(this->serializerDoc,*this->serial);
        this->serial->println();
    }

private:
    static ComHandler* instance;
    JsonDocument msgDoc;
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