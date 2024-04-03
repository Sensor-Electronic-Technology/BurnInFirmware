#include "ComHandler.hpp"
#include "../Logging/StationLogger.hpp"
#include "../Files/FileManager.hpp"

ComHandler* ComHandler::instance=nullptr;

void ComHandler::MsgPacketDeserialize() {
    JsonDocument serialEventDoc;
    const char* prefix=serialEventDoc[F("Prefix")].as<const char*>();
    bool found=false;
    PacketType packetType;    
    for(uint8_t i=0;i<15;i++){
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
                auto packet=serialEventDoc[F("Packet")].as<JsonObject>();
                ProbeControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                break;
            }
            case PacketType::HEATER_CONFIG:{
                auto packet=serialEventDoc[F("Packet")].as<JsonObject>();
                HeaterControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                break;
            }
            case PacketType::SYSTEM_CONFIG:{
                auto packet=serialEventDoc[F("Packet")].as<JsonObject>();
                // serializeJson(serialEventDoc,*this->serial);
                ControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                //this->InstanceMsgPacketSerializer(config,packetType);
                break;
            }
            case PacketType::COMMAND:{
                auto packet=serialEventDoc[F("Packet")].as<JsonObject>();
                StationCommand command=(StationCommand)serialEventDoc[F("Packet")];
                this->_commandCallback(command);
                break;
            }
            case PacketType::ID_REQUEST:{
                this->SendId();
                break;
            }
            case PacketType::ID_RECEIVE:{
                this->ReceiveId(serialEventDoc);
                break;
            }
            case PacketType::VER_REQUEST:{
                this->SendVersion();
                break;
            }
            case PacketType::VER_RECIEVE:{
                this->ReceiveVersion(serialEventDoc);
                break;
            }
            case PacketType::HEATER_TUNE_RESPONSE:{
                auto response=serialEventDoc[F("Packet")].as<Response>();
                this->heaterResponseCb(response);
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
}

void ComHandler::SendId(){
    JsonDocument serializerDoc;
    EEPROM_read(ID_ADDR,StationId);
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::ID_REQUEST);
    serializerDoc[F("Packet")]=StationId;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::ReceiveId(const JsonDocument& serialEventDoc){
    auto id=serialEventDoc[F("Packet")].as<const char*>();
    sprintf(StationId,id);
    EEPROM_write(ID_ADDR,StationId);
}

void ComHandler::SendVersion(){
    JsonDocument serializerDoc;
    EEPROM_read(VER_ADDR,FirmwareVersion);
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::VER_REQUEST);
    serializerDoc[F("Packet")]=FirmwareVersion;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendStartResponse(bool success,const char* message){
    JsonDocument serializerDoc;
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::TEST_START_STATUS);
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Status")]=success;
    packet[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
}

void ComHandler::InstanceSendTestCompleted(const char* message){
    JsonDocument serializerDoc;
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::TEST_COMPLETED);
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendTestStartFromLoad(bool success,const char* message){
    JsonDocument serializerDoc;
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::TEST_LOAD_START);
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Status")]=success;
    packet[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::ReceiveVersion(const JsonDocument& serialEventDoc){
    auto version=serialEventDoc[F("Packet")].as<const char*>();
    sprintf(FirmwareVersion,version);
    EEPROM_write(VER_ADDR,FirmwareVersion);
}

// void ComHandler::ResponseDeserializer(JsonDocument& doc){
//     auto packet=doc[F("Packet")].as<JsonObject>();
//     Response response=(Response)packet[F("Response")];
//     switch(response){
//         case Response::HEATER_SAVE:
//         case Response::HEATER_CANCEL:{
//             this->heaterResponseCb(response);
//             break;
//         }
//         case Response::TEST_CONTINUE:
//         case Response::TEST_CANCEL:{
//             this->testResponseCb(response);
//             break;
//         }
//     }
// }

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
    JsonDocument serializerDoc;
    Derived_from<T,Serializable>();
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    data.Serialize(&packet,true);
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
}

template <typename T> 
void ComHandler::InstanceSendRequest(PacketType packetType,const char* request,const T& data) {
    JsonDocument serializerDoc;
    Derived_from<T,Serializable>();
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(packetType);
    serializerDoc[F("RequestText")]=request;
    auto packet=serializerDoc[F("Packet")].to<JsonObject>();
    data.Serialize(&packet,true);
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendMessage(const char* message){
    JsonDocument serializerDoc;
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::MESSAGE);
    auto packetJson=serializerDoc[F("Packet")].to<JsonObject>();
    packetJson[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendInitMessage(const char* message){
    JsonDocument serializerDoc;
    serializerDoc.clear();
    serializerDoc[F("Prefix")]=read_packet_prefix(PacketType::INIT);
    auto packetJson=serializerDoc[F("Packet")].to<JsonObject>();
    packetJson[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

