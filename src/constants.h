#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoComponents.h>
#include <avr/pgmspace.h>
#include <SdFat.h>

template <class T> int EEPROM_write(int addr, const T& value) {
    const byte* p = (const byte*)(const void*)&value;
    int newAddr;
    for (newAddr = 0; newAddr < sizeof(value); newAddr++) {
        EEPROM.write(addr++, *p++);
    }
    return newAddr;
}//End write any value/type

template <class T> int EEPROM_read(int addr, T& value) {
    byte* p = (byte*)(void*)&value;
    int newAddr;
    for (newAddr = 0; newAddr < sizeof(value); newAddr++)
        *p++ = EEPROM.read(addr++);
    return newAddr;
}//End read any value/type

#pragma region Globals

    inline char StationId[4];
    inline char FirmwareVersion[8];
    inline bool sdInitialized=false;

#pragma endregion

#define ID_ADDR     10
#define VER_ADDR    64

#pragma region MACROS

#define read_file_message(msg)      ((const char *)pgm_read_ptr(&(file_result_messages[msg])))
#define read_system_message(msg)    ((const char *)pgm_read_ptr(&(system_message_table[msg])))
#define read_system_error(msg)      ((const char *)pgm_read_ptr(&(system_error_table[msg])))

#define read_log_prefix(pre)        ((const char *)pgm_read_ptr(&(log_level_prefixes[pre])))
#define read_packet_prefix(pre)     ((const char *)pgm_read_ptr(&(prefixes[pre])))
#define read_filename(pType)        ((const char *)pgm_read_ptr(&(json_filenames[pType])))
/* #define read_state_filename()     ((const char *)pgm_read_ptr(&(json_filenames[3]))) */

#pragma endregion

#pragma region Constants
    #define SD_FAT_TYPE 3
    #define SPI_CLOCK SD_SCK_MHZ(50)
    #define SD_CS_PIN SS
    #if HAS_SDIO_CLASS
    #define SD_CONFIG SdioConfig(FIFO_SDIO)
    #elif ENABLE_DEDICATED_SPI
    #define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
    #else  // HAS_SDIO_CLASS
    #define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
    #endif  // HAS_SDIO_CLASS


    #define TEST_ID_LENGTH                 24
    //Timer
    #define TIMER_PERIOD                   1
    #define TIMER_FACTOR                   1000
    #define TIME_INIT_TEST                 120000ul

    //LED Control
    #define PIN_CURRENT		               2
    #define PIN_CURRENT_120mA		       6
    #define PIN_CURRENT_60mA               7

    #define TIME_SECS_120mA                 72000ul
    #define TIME_SECS_60mA                  TIME_SECS_120mA
    #define TIME_SECS_150mA                 25200ul

    #define TEMP_INTERVAL                   100ul   
    #define COM_INTERVAL                    250ul  //250ms
    #define LOG_INTERVAL                    300000ul //5min
    #define VER_CHECK_INTERVAL              3600000ul //1hr
    #define UPDATE_INTERVAL                 500ul  //500ms
    #define DEFAULT_CURRENT                 CurrentValue::c150
    #define DEFAULT_FWEIGHT                 0.1
    #define TIME_OFF_PERCENT                98.5f

    #define ADC_MIN                         0
    #define ADC_MAX                         1023

    #define VERSION_PERIOD                  10000ul //10sec
    #define ID_PERIOD                       5000ul //5sec
#pragma endregion

#pragma region ENUMS

    enum ConfigType:uint8_t{
        HEATER_CONFIG=0,            //PC to Station config
        PROBE_CONFIG=1,             //PC to stattion config
        SYSTEM_CONFIG=2,            //PC to station config
    };

    enum FileResult:uint8_t{
        DOES_NOT_EXIST,
        FAILED_TO_OPEN,
        DESERIALIZE_FAILED,
        LOADED,
        SAVED,
        FAILED_TO_SERIALIZE,
        FAILED_TO_SAVE,
        SD_NOT_INITIALIZED
    };

    enum StationCommand:uint8_t{
        START=0,
        PAUSE=1,
        CONTINUE=2,
        TOGGLE_HEAT=3,
        CYCLE_CURRENT=4,
        PROBE_TEST=5,
        CHANGE_MODE_ATUNE=6,
        CHANGE_MODE_NORMAL=7,
        START_TUNE=8,
        STOP_TUNE=9,
        SAVE_TUNE=10,
        CANCEL_TUNE=11,
        RESET=12,
        REQUEST_RUNNING_TEST=13,
        FORMAT_SD=14,
    };

    enum AckType:uint8_t{
        TEST_START_ACK=0,
        VER_ACK=1,
        ID_ACK=2,
        TEST_FINISH_ACK=3,   
    };

#pragma endregion

#pragma region Callbacks
    typedef components::Function<void(void)> RestartRequiredCallback;
    typedef components::Function<void(StationCommand)> CommandCallback;
    typedef components::Function<void(void)> TestFinsihedCallback;
    typedef components::Function<void(void)> TestAckCallback;
    typedef components::Function<void(AckType)> AckCallback;
    typedef components::Function<void(const char* testId)> TestIdCallback;
    typedef components::Function<void(ConfigType)> GetConfigCallback;
    typedef components::Function<void(unsigned long)> ReceiveWindowSizeCallback;
    
#pragma endregion

#pragma region PrefixDefinitions
    #define PREFIX_COUNT    25

    enum PacketType:uint8_t{
        SAVE_STATE=0,               //???
        MESSAGE=1,                  //outgoing->message of speficied type
        DATA=2,                     //outgoing-> readings and state data
        COMMAND=3,                  //incoming-> route to Controller
        ID_RECEIVE=4,               //Set station id
        ID_REQUEST=5,               //Request station id-Send to PC
        VER_RECIEVE=6,              //incoming-> new firmware version
        VER_REQUEST=7,             //incoming-> request version and send
        TEST_START_STATUS=8,       //outgoing->Notify PC that test has started
        TEST_COMPLETED=9,          //outgoing->Notify PC that test has completed
        TEST_LOAD_START=10,         //outgoing->Notify PC that test is starting from a load state
        HEATER_NOTIFY=11,           //Outgoing->Notify PC of a single heater tuning results
        HEATER_TUNE_COMPLETE=12,    //Outgoing->notfiy PC that all heaters have been tuned,
        ACK=13,                     //Outgoing->Acknowledge PC of command
        TUNE_COM=14,                //Outgoing->tuning serial data,
        SEND_TEST_ID=15,            //Incoming->Start test
        LOAD_STATE=16,              //Incoming->Load saved state
        CONFIG_SAVE_STATUS=17,       //Outgoing->Notify PC of config save status
        GET_CONFIG=18,               //Incoming->Request config
        RECEIVE_CONFIG=19,           //Outgoing->Send config
        PROBE_TEST_DONE=20,         //Outgoing->Notify PC that probe test is done
        REQUEST_CONFIG_BACKUP=21,   //Incoming->Request config backup,
        SEND_RUNNING_TEST=22,       //Incoming->Request running test,
        NOTIFY_SW_HEATER_MODE=23,   //Outgoing->Notify PC that in tuning mode,
        RECEIVE_WINDOW_SIZE=24     //Incoming->PID tune window size
    };
    const char strPre_00[] PROGMEM="ST";   
    const char strPre_01[] PROGMEM="M";    
    const char strPre_02[] PROGMEM="D";    
    const char strPre_03[] PROGMEM="COM";  
    const char strPre_04[] PROGMEM="IDREC";  
    const char strPre_05[] PROGMEM="IDREQ";   
    const char strPre_06[] PROGMEM="VERREC";  
    const char strPre_07[] PROGMEM="VERREQ";  
    const char strPre_08[] PROGMEM="TSTAT";     
    const char strPre_09[] PROGMEM="TCOMP";     
    const char strPre_10[] PROGMEM="TLOAD";     
    const char strPre_11[] PROGMEM="HNOTIFY";
    const char strPre_12[] PROGMEM="HTUNED";
    const char strPre_13[] PROGMEM="ACK";
    const char strPre_14[] PROGMEM="TCOM";
    const char strPre_15[] PROGMEM="TID";
    const char strPre_16[] PROGMEM="LSTATE";
    const char strPre_17[] PROGMEM="SCONF";
    const char strPre_18[] PROGMEM="GCONF";
    const char strPre_19[] PROGMEM="RCONF";
    const char strPre_20[] PROGMEM="PTD";
    const char strPre_21[] PROGMEM="RCONFB";
    const char strPre_22[] PROGMEM="RTEST";;
    const char strPre_23[] PROGMEM="SWHEATER";
    const char strPre_24[] PROGMEM="WINSIZE";

    const char* const prefixes[] PROGMEM = {
        strPre_00,
        strPre_01,
        strPre_02,
        strPre_03,
        strPre_04,
        strPre_05,
        strPre_06,
        strPre_07,
        strPre_08,
        strPre_09,
        strPre_10,
        strPre_11,
        strPre_12,
        strPre_13,
        strPre_14,
        strPre_15,
        strPre_16,
        strPre_17,
        strPre_18,
        strPre_19,
        strPre_20,
        strPre_21,
        strPre_22,
        strPre_23,
        strPre_24
    };
#pragma endregion

#pragma region FileNames
    const char strFile_01[] PROGMEM="/hConfigs.txt";  //0
    const char strFile_02[] PROGMEM="/pConfigs.txt";  //1
    const char strFile_03[] PROGMEM="/sConfig.txt";   //2
    const char strFile_04[] PROGMEM="/state.txt";      //3

    const char* const json_filenames[] PROGMEM = {
        strFile_01,
        strFile_02,
        strFile_03
    };

#pragma endregion

#pragma region SystemMessages
    enum MessageType:uint8_t{
        GENERAL,
        INIT,
        NOTIFY,
        ERROR
    };

    enum SystemMessage:uint8_t{
        BEFORE_FREE_MEM=0,//
        FIRMWARE_INIT=1,  //
        LOAD_CONFIG=2,   //
        AFTER_FREE_MEM=3, //
        CONFIG_LOADED=4, //
        COMPONENT_INIT=5, //
        HEATERS_INIT=6,
        PROBES_INIT=7,
        REG_COMPONENTS=8,
        COMPONENTS_INIT_COMPLETE=9,
        TIMER_INIT=10,
        TIMER_INIT_COMPLETE=11,
        CHECK_SAVED_STATE=12,
        STATE_LOADED=13,
        NO_SAVED_STATE=14,
        FIRMWARE_INIT_COMPLETE=15,
        TUNING_RESULT_SAVED=16,
        TUNING_CANCELD=17,
        DELETE_SAVED_STATE=18,
        SAVED_STATE_DELETED=19,
        RESETTING_CONTROLLER=20,
        SD_INIT=21,
        TEST_STATE_TRANSITION=22,
        TEST_STATE_COMPLETED=23,
        HEATER_MODE_ATUNE=24,
        HEATER_MODE_HEATING=25,
        HEATER_STATE_TRANSITION=26,
        PROBE_TEST_START=27,
        PROBE_TEST_END=28,
        CURRENT_CHANGED=29,
        SD_FORMATTED=30
    };

    const char str_00[] PROGMEM=    "Before Loading Free Memory: %d";  
    const char str_01[] PROGMEM=   "Firmware Initialization Starting";  
    const char str_02[] PROGMEM=   "Loading configuration files";  
    const char str_03[] PROGMEM=   "After Loading Free Memory: %d";  
    const char str_04[] PROGMEM=   "Configuration Files Loaded";  
    const char str_05[] PROGMEM=    "Components Initalizing Starting";  
    const char str_06[] PROGMEM=   "Heaters initialized";  
    const char str_07[] PROGMEM=   "Probes initialized";  
    const char str_08[] PROGMEM=   "Registering Components"; 
    const char str_09[] PROGMEM=    "Components Initalized"; 
    const char str_10[] PROGMEM=    "Initalizing Timers"; 
    const char str_11[] PROGMEM=    "Timer Initialization Complete"; 
    const char str_12[] PROGMEM=    "Check for saved state, Free Memory: %d";
    const char str_13[] PROGMEM=    "Saved state found, state loaded. Resuming from saved state";
    const char str_14[] PROGMEM=    "No saved state found, continuing to normal operation";
    const char str_15[] PROGMEM=    "Firmware Initialization Complete"; 
    const char str_16[] PROGMEM=    "Tuning results saved"; 
    const char str_17[] PROGMEM=    "Tuning canceled"; 
    const char str_18[] PROGMEM=    "Deleting saved state"; 
    const char str_19[] PROGMEM=    "Saved state deleted"; 
    const char str_20[] PROGMEM=    "Controller resetting, please wait"; 
    const char str_21[] PROGMEM=    "SD Card Initialized"; 
    const char str_22[] PROGMEM=    "TestController Transition from StateId %d from StateId %d"; 
    const char str_23[] PROGMEM=    "Test Completed";
    const char str_24[] PROGMEM=    "Heater mode switched to auto tune";
    const char str_25[] PROGMEM=    "Heater mode switched to heating";
    const char str_26[] PROGMEM=    "TestController Transition from StateId %d from StateId %d"; 
    const char str_27[] PROGMEM=    "Probe Test Started,Current: %d"; 
    const char str_28[] PROGMEM=    "Probe Test Completed,Current Restored to %d"; 
    const char str_29[] PROGMEM=    "Current Changed to %d"; 
    const char str_30[] PROGMEM=    "SD Card Formatted, please re-upload configurations before tuning or running tests"; 

    const char* const system_message_table[] PROGMEM={
        str_00,
        str_01,
        str_02,
        str_03,
        str_04,
        str_05,
        str_06,
        str_07,
        str_08,
        str_09,
        str_10,
        str_11,
        str_12,
        str_13,
        str_14,
        str_15,
        str_16,
        str_17,
        str_18,
        str_19,
        str_20,
        str_21,
        str_22,
        str_23,
        str_24,
        str_25,
        str_26,
        str_27,
        str_28,
        str_29,
        str_30
    };


#pragma endregion

#pragma region SystemErrorMessages

    enum SystemError:uint8_t{
        CONFIG_LOAD_FAILED=0,
        CONFIG_LOAD_FAILED_FILE=1,
        CONFIG_SAVE_FAILED=2,
        CONFIG_SAVE_FAILED_FILE=3,
        SD_INIT_FAILED=4,
        HEATER_SAVE_FAILED=5,
        SAVED_STATE_FAILED=6,
        STATE_DELETE_FAILED=7,
        INVALID_COMMAND=8,
        FAILED_DESERIALIZE=9,
        FAILED_SERIALIZE=10,
        INVALID_PREFIX=11,
        PREFIX_NOT_FOUND=12,
        TEST_STATE_TRANSITION_ERR=13,
        START_ALREADY_RUNNING=14,
        START_NOT_SET=15,
        PAUSE_ALREADY_PAUSED=16,
        CONTINUE_NOT_PAUSED=17,
        TUNE_STOP_ERR=18,
        TUNE_START_ERR=19,
        TUNE_SAVE_CMD_ERR=20,
        TUNE_DISCARD_CMD_ERR=21,
        TUNE_TRANSISITON_ERR=22,
        HEATER_TRANSITION_ERR=23,
        CHANGE_RUNNING_ERR=24,
        MAX_TEMP_ERR=25,
        TEST_RUNNING_ERR=26,
        TEST_ID_NOT_SET=27,
        SD_FORMAT_FAILED=28,
        TUNE_SAVE_FAILED=29,
        TUNE_WINDOW_FAILED=30,
    };

    const char strErr_00[] PROGMEM="Failed to load configuration files. Please contact administrator";
    const char strErr_01[] PROGMEM="Failed to load configuration file: %s. Defaults will be loaded. \n Please contact administarator";
    const char strErr_02[] PROGMEM="Failed to save configuration files.  Please contact administrator";
    const char strErr_03[] PROGMEM="Failed to save configuration file: %s. Changes will be lost on reset. Please contact administrator";
    const char strErr_04[] PROGMEM="SD card failed to initialize!! Defaults will be loaded. \n If you continue no changes to the configuration will be saved. Please check connections and contact administrator";
    const char strErr_05[] PROGMEM="Failed to save heater tuning results!! Please contact administrator";
    const char strErr_06[] PROGMEM="Failed to load saved state, station will continue to normal operation. Please contact administrator";
    const char strErr_07[] PROGMEM="Failed to delete saved state! Please contact administrator";
    const char strErr_08[] PROGMEM="Invalid command recieved! Please contact administrator";
    const char strErr_09[] PROGMEM="Failed to deserialize data: %s. Please contact administrator";
    const char strErr_10[] PROGMEM="Failed to serialize data: %s. Please contact administrator";
    const char strErr_11[] PROGMEM="Invalid message packet prefix recieved: %s";
    const char strErr_12[] PROGMEM="Prefix not found in message packet";
    const char strErr_13[] PROGMEM="Failed to transition to idle from running state, test is being hard stopped. \n restart controller before starting another test";
    const char strErr_14[] PROGMEM="Failed to start, test is already running";
    const char strErr_15[] PROGMEM="Failed to start, current or saveState was not set";
    const char strErr_16[] PROGMEM= "Failed to pause, test is already paused";
    const char strErr_17[] PROGMEM="Failed to continue, test is not paused";
    const char strErr_18[] PROGMEM="Failed to stop tuning, tuning is either in idle or complete state.";
    const char strErr_19[] PROGMEM="Failed to start tuning, tuning is running or complete state.";
    const char strErr_20[] PROGMEM="Failed to save tuning results, tuning is not in complete state";
    const char strErr_21[] PROGMEM="Failed to discard tuning, tuning is not in complete state";
    const char strErr_22[] PROGMEM="Error: Not in Tuning Mode, Switch modes to start tuning";
    const char strErr_23[] PROGMEM="Error: Not in Heating Mode, Switch modes to Turn On/Off Heaters";
    const char strErr_24[] PROGMEM="Error: Cannot change current or temperature while a test is running";
    const char strErr_25[] PROGMEM="Error: Temperature set point must be <= %d";
    const char strErr_26[] PROGMEM="Error: Cannot perform this action while a test is running";
    const char strErr_27[] PROGMEM="Error: Test Id must be set before starting a test";
    const char strErr_28[] PROGMEM="Error: Failed to format SD Card, ErrorCode: %d ErrorData: %d";
    const char strErr_29[] PROGMEM="Error: Failed to save tuning results.  Please try again discard results";
    const char strErr_30[] PROGMEM="Error: Failed to set tuning window size, tuning is currently running. To set the tuning please stop the current tune process and try again";

    const char* const system_error_table[] PROGMEM={
        strErr_00,
        strErr_01,
        strErr_02,
        strErr_03,
        strErr_04,
        strErr_05,
        strErr_06,
        strErr_07,
        strErr_08,
        strErr_09,
        strErr_10,
        strErr_11,
        strErr_12,
        strErr_13,
        strErr_14,
        strErr_15,
        strErr_16,
        strErr_17,
        strErr_18,
        strErr_19,
        strErr_20,
        strErr_21,
        strErr_22,
        strErr_23,
        strErr_24,
        strErr_25,
        strErr_26,
        strErr_27,
        strErr_28,
        strErr_29,
        strErr_30
    };

#pragma endregion