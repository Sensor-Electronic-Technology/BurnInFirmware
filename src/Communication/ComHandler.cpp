#include "ComHandler.hpp"
#include "../Logging/StationLogger.hpp"
#include "../Files/FileManager.hpp"

ComHandler* ComHandler::instance=nullptr;

void ComHandler::MsgPacketDeserialize() {
    const char* prefix=this->serialEventDoc[F("Prefix")].as<const char*>();
    //StationLogger::Log(LogLevel::INFO,true,false,F("Input Prefix: %s",prefix);
    bool found=false;
    PacketType packetType;    
    for(uint8_t i=0;i<13;i++){
        const char* target=read_packet_prefix(i);
        if(strcmp(prefix,target)==0){
            found=true;
            packetType=(PacketType)i;
            break;
        }
    }
    if(found){
        switch(packetType){
            case PacketType::PROBE_CONFIG:{
                auto packet=this->serialEventDoc[F("Packet")].as<JsonObject>();
                ProbeControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                break;
            }
            case PacketType::HEATER_CONFIG:{
                auto packet=this->serialEventDoc[F("Packet")].as<JsonObject>();
                HeaterControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                break;
            }
            case PacketType::SYSTEM_CONFIG:{
                auto packet=this->serialEventDoc[F("Packet")].as<JsonObject>();
                serializeJson(this->serialEventDoc,*this->serial);
                ControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                //this->InstanceMsgPacketSerializer(config,packetType);
                break;
            }
            case PacketType::COMMAND:{
                //StationLogger::Log(LogLevel::INFO,true,false,F("Packet found.  Packet: %s"),prefix);
                auto packet=this->serialEventDoc[F("Packet")].as<JsonObject>();
                StationCommand command=(StationCommand)this->serialEventDoc[F("Packet")];
                this->_commandCallback(command);
                break;
            }
            case PacketType::HEATER_RESPONSE:{
                break;
            }
            case PacketType::TEST_RESPONSE:{
                //this->ResponseDeserializer(this->serialEventDoc);
                break;
            }
            case PacketType::ID_REQUEST:{
                //StationLogger::Log(LogLevel::INFO,true,false,F("Packet found.  Packet: %s"),prefix);
                this->SendId();
                break;
            }
            case PacketType::ID_RECEIVE:{
                //StationLogger::Log(LogLevel::INFO,true,false,F("Packet found.  Packet:  %s"),prefix);
                this->ReceiveId();
                break;
            }
            default:{
                StationLogger::Log(LogLevel::ERROR,true,false,F("Invalid prefix.. Prefix: %s"),prefix);
                break;
            }
        }
    }else{
        StationLogger::Log(LogLevel::ERROR,true,false,F("Prefix not found.."));
    }
    this->serialEventDoc.clear();
}

void ComHandler::SendId(){
    EEPROM_read(ID_ADDR,StationId);
    this->serializerDoc.clear();
    this->serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::ID_REQUEST);
    this->serializerDoc[F("Packet")]=StationId;
    serializeJson(this->serializerDoc,*this->serial);
    this->serial->println();
    this->serializerDoc.clear();
}

void ComHandler::ReceiveId(){
    auto id=this->serialEventDoc[F("Packet")].as<const char*>();
    sprintf(StationId,id);
    EEPROM_write(ID_ADDR,StationId);
}

void ComHandler::ResponseDeserializer(JsonDocument& doc){
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
void ComHandler::InstanceMsgPacketSerializer(const T& data,PacketType packetType) {
    Derived_from<T,Serializable>();
    this->serializerDoc.clear();
    this->serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
    JsonObject packet=this->serializerDoc[F("Packet")].to<JsonObject>();
    data.Serialize(&packet,true);
    serializeJson(this->serializerDoc,*this->serial);
    this->serial->println();
    this->serialEventDoc.clear();
}

template <typename T> 
void ComHandler::InstanceSendRequest(PacketType packetType,const char* request,const T& data) {
    Derived_from<T,Serializable>();
    this->serializerDoc.clear();
    this->serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
    this->serializerDoc[F("RequestText")]=request;
    auto packet=this->serializerDoc[F("Packet")].to<JsonObject>();
    data.Serialize(&packet,true);
    serializeJson(this->serializerDoc,*this->serial);
    this->serial->println();
    this->serializerDoc.clear();
}

void ComHandler::InstanceSendMessage(const char* message){
    this->serializerDoc.clear();
    this->serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::MESSAGE);
    auto packetJson=this->serializerDoc[F("Packet")].to<JsonObject>();
    packetJson[F("Message")]=message;
    serializeJson(this->serializerDoc,*this->serial);
    this->serial->println();
    this->serializerDoc.clear();
}

