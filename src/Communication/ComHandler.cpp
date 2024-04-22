#include "ComHandler.hpp"
#include "../Files/FileManager.hpp"

ComHandler* ComHandler::instance=nullptr;

void ComHandler::MsgPacketDeserialize(JsonDocument& serialEventDoc) {
    const char* prefix=serialEventDoc[F("Prefix")].as<const char*>();
    bool found=false;
     PacketType packetType;
    for(uint8_t i=0;i<PREFIX_COUNT;i++){
        char target[BUFFER_SIZE];
        strcpy_P(target,read_packet_prefix(i));
        if(strcmp(prefix,(const char*)target)==0){
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
                ControllerConfig config;
                config.Deserialize(packet);
                FileManager::Save(&config,packetType);
                break;
            }
            case PacketType::COMMAND:{
                auto packet=serialEventDoc[F("Packet")].as<JsonObject>();
                StationCommand command=(StationCommand)serialEventDoc[F("Packet")];
                this->_commandCallback(command);
                break;
            }
            case PacketType::ID_REQUEST:{
                this->InstanceSendId();
                break;
            }
            case PacketType::ID_RECEIVE:{
                this->ReceiveId(serialEventDoc);
                break;
            }
            case PacketType::VER_REQUEST:{
                this->InstanceSendVersion();
                break;
            }
            case PacketType::VER_RECIEVE:{
                this->ReceiveVersion(serialEventDoc);
                break;
            }
            case PacketType::ACK:{
                auto ackType=serialEventDoc[F("Packet")].as<AckType>();
                this->_ackCallback(ackType);
                break;
            }
            case PacketType::UPDATE_CURRENT:{
                auto current=serialEventDoc[F("Packet")].as<int>();
                this->_changeCurrentCallback(current);
                break;
            }
            case PacketType::UPDATE_TEMP:{
                auto temp=serialEventDoc[F("Packet")].as<int>();
                this->_changeTempCallback(temp);
                break;
            }
            case PacketType::SEND_TEST_ID:{
                auto testId=serialEventDoc[F("Packet")].as<const char*>();
                this->_testIdCallback(testId);
                break;
            }
            default:{
                ComHandler::SendErrorMessage(SystemError::INVALID_PREFIX,prefix);
                break;
            }
        }
    }else{
        ComHandler::SendErrorMessage(SystemError::PREFIX_NOT_FOUND);
    }
}

void ComHandler::InstanceSendId(){
    JsonDocument serializerDoc;
    EEPROM_read(ID_ADDR,StationId);
    serializerDoc.clear();
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::ID_REQUEST));
    serializerDoc[F("Prefix")]=packetStr;
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

void ComHandler::InstanceSendVersion(){
    JsonDocument serializerDoc;
    EEPROM_read(VER_ADDR,FirmwareVersion);
    serializerDoc.clear();
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::VER_REQUEST));
    serializerDoc[F("Prefix")]=packetStr;
    serializerDoc[F("Packet")]=FirmwareVersion;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendTestCompleted(const char* message){
    JsonDocument serializerDoc;
    serializerDoc.clear();
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::TEST_COMPLETED));
    serializerDoc[F("Prefix")]=packetStr;
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendTestStartFromLoad(bool success,const char* message,const char* testId){
    JsonDocument serializerDoc;
    serializerDoc.clear();
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::TEST_LOAD_START));
    serializerDoc[F("Prefix")]=packetStr;
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Status")]=success;
    packet[F("Message")]=message;
    packet[F("TestId")]=testId;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::ReceiveVersion(const JsonDocument& serialEventDoc){
    auto version=serialEventDoc[F("Packet")].as<const char*>();
    sprintf(FirmwareVersion,version);
    EEPROM_write(VER_ADDR,FirmwareVersion);
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
    JsonDocument serializerDoc;
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(packetType));
    serializerDoc[F("Prefix")]=packetStr;
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
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(packetType));
    serializerDoc[F("Prefix")]=packetStr;
    serializerDoc[F("RequestText")]=request;
    auto packet=serializerDoc[F("Packet")].to<JsonObject>();
    data.Serialize(&packet,true);
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendStartResponse(bool success,const char* message){
    JsonDocument serializerDoc;
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::TEST_START_STATUS));
    serializerDoc[F("Prefix")]=packetStr;
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Status")]=success;
    packet[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
}


