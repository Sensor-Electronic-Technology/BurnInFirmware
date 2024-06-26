#include "ComHandler.hpp"
#include "../Files/FileManager.hpp"
#include "../free_memory.h"

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
            case PacketType::RECEIVE_CONFIG:{
                this->InstanceReceiveConfig(serialEventDoc);
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
            case PacketType::SEND_TEST_ID:{
                auto testId=serialEventDoc[F("Packet")].as<const char*>();
                this->_testIdCallback(testId);
                break;
            }
            case PacketType::LOAD_STATE:{
                auto state=serialEventDoc[F("Packet")].as<JsonObject>();
                SaveState saveState;
                saveState.Deserialize(state);
                this->_loadStateCallback(saveState);
                break;
            }
            case PacketType::GET_CONFIG:{
                auto configType=serialEventDoc[F("Packet")].as<ConfigType>();
                this->_getConfigCallback(configType);
                break;
            }
            case PacketType::RECEIVE_WINDOW_SIZE:{
                auto windowSize=serialEventDoc[F("Pakcet")].as<int>();
                this->_receiveWindowSizeCallback(windowSize);
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

void ComHandler::InstanceReceiveConfig(JsonDocument& serialEventDoc){
    auto packet=serialEventDoc[F("Packet")].as<JsonObject>();
    ConfigType configType=packet[F("ConfigType")].as<ConfigType>();
    this->serial->println("Free Ram: "+String(FreeSRAM()));
    auto configJson=packet[F("Configuration")].as<JsonObject>();
    switch(configType){
        case ConfigType::PROBE_CONFIG:{
            ProbeControllerConfig config;
            config.Deserialize(configJson);
            auto success=FileManager::SaveConfiguration(&config,ConfigType::PROBE_CONFIG);
            delay(500);
            if(success){
                this->InstanceSendConfigSaved(configType,F("ProbeControllerConfig Saved"),true);
                this->_restartRequiredCallback();
            }else{
                this->InstanceSendConfigSaved(configType,F("ProbeControllerConfig Saved"),false);
            }
            break;
        }
        case ConfigType::HEATER_CONFIG:{
            HeaterControllerConfig config;
            config.Deserialize(configJson);
            auto success=FileManager::SaveConfiguration(&config,ConfigType::HEATER_CONFIG);
            delay(500);
            if(success){
                this->InstanceSendConfigSaved(configType,F("HeaterControllerConfig Saved"),true);
                this->_restartRequiredCallback();
            }else{
                this->InstanceSendConfigSaved(configType,F("HeaterControllerConfig Saved"),false);
            }
            break;
        }
        case ConfigType::SYSTEM_CONFIG:{
            ControllerConfig config;
            config.Deserialize(configJson);
            auto success=FileManager::SaveConfiguration(&config,ConfigType::SYSTEM_CONFIG);
            if(success){
                this->InstanceSendConfigSaved(configType,F("ControllerConfig Saved"),true);
                delay(500);
                this->_restartRequiredCallback();
            }else{
                this->InstanceSendConfigSaved(configType,F("ControllerConfig Saved"),false);
            }
            break;
        }
        default:{
            ComHandler::SendErrorMessage(SystemError::CONFIG_SAVE_FAILED,"Invalid Config Type");
            break;
        }
    }
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

void ComHandler::InstanceSendProbeTestDone() {
    JsonDocument serializerDoc;
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::PROBE_TEST_DONE));
    serializerDoc[F("Prefix")]=packetStr;
    serializerDoc[F("Packet")]=F("Probe Test Done");
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
}

void ComHandler::InstanceNotifyHeaterMode(HeaterMode mode) {
    JsonDocument serializerDoc;
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::NOTIFY_SW_HEATER_MODE));
    serializerDoc[F("Prefix")]=packetStr;
    serializerDoc[F("Packet")]=(uint8_t)mode;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
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

void ComHandler::InstanceSendTestCompleted(const char* message){
    JsonDocument serializerDoc;
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::TEST_COMPLETED));
    serializerDoc[F("Prefix")]=packetStr;
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Message")]=message;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
    serializerDoc.clear();
}

void ComHandler::InstanceSendConfigSaved(ConfigType configType,const __FlashStringHelper* msg, bool success){
    JsonDocument serializerDoc;
    char buffer[BUFFER_SIZE];
    PGM_P msgMem=reinterpret_cast<PGM_P>(msg);
    strcpy_P(buffer,msgMem);
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::CONFIG_SAVE_STATUS));
    serializerDoc[F("Prefix")]=packetStr;
    JsonObject packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("Type")]=configType;
    packet[F("Message")]=buffer;
    packet[F("Status")]=success;
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
}

void ComHandler::InstanceSendConfig(ConfigType configType,Serializable* config){
    JsonDocument serializerDoc;
    char packetStr[BUFFER_SIZE];
    strcpy_P(packetStr,read_packet_prefix(PacketType::GET_CONFIG));
    serializerDoc[F("Prefix")]=packetStr;
    auto packet=serializerDoc[F("Packet")].to<JsonObject>();
    packet[F("ConfigType")]=configType;
    auto configJson=packet[F("Configuration")].to<JsonObject>();
    switch(configType){
        case ConfigType::HEATER_CONFIG:{
            HeaterControllerConfig heaterConfig=static_cast<HeaterControllerConfig&>(*config);
            heaterConfig.Serialize(&configJson,true);
            break;
        }
        case ConfigType::PROBE_CONFIG:{
            ProbeControllerConfig probeConfig=static_cast<ProbeControllerConfig&>(*config);
            probeConfig.Serialize(&configJson,true);
            break;
        }
        case ConfigType::SYSTEM_CONFIG:{
            ProbeControllerConfig controllerConfig=static_cast<ProbeControllerConfig&>(*config);
            controllerConfig.Serialize(&configJson,true);
            break;
        }
    }
    serializeJson(serializerDoc,*this->serial);
    this->serial->println();
}




