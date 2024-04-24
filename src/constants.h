#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoComponents.h>
#include <avr/pgmspace.h>

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

#define read_file_message(msg)   ((const char *)pgm_read_ptr(&(file_result_messages[msg])))
#define read_system_message(msg) ((const char *)pgm_read_ptr(&(system_message_table[msg])))
#define read_system_error(msg)   ((const char *)pgm_read_ptr(&(system_error_table[msg])))

#define read_log_prefix(pre)      ((const char *)pgm_read_ptr(&(log_level_prefixes[pre])))
#define read_packet_prefix(pre)   ((const char *)pgm_read_ptr(&(prefixes[pre])))
#define read_filename(pType)      ((const char *)pgm_read_ptr(&(json_filenames[pType])))

#pragma endregion


#pragma region Constants
    #define TEST_ID_LENGTH                  24
    //Timer
    #define TIMER_PERIOD                   1
    #define TIMER_FACTOR                   1000

    //LED Control
    #define PIN_CURRENT		               2
    #define PIN_CURRENT_120mA		       6
    #define PIN_CURRENT_60mA               7

    #define TIME_SECS_120mA                 72000ul
    #define TIME_SECS_60mA                  TIME_SECS_120mA
    #define TIME_SECS_150mA                 25200ul


    #define TEMP_INTERVAL                   100ul   
    #define COM_INTERVAL                    1000ul  //1sec
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
        RESET=12
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
    typedef components::Function<void(int)> ChangeCurrentCallback;
    typedef components::Function<void(int)> ChangeTempCallback;
    typedef components::Function<void(const char* testId)> TestIdCallback;
    
#pragma endregion

#pragma region PrefixDefinitions
    #define PREFIX_COUNT    22

    enum PacketType:uint8_t{
        HEATER_CONFIG=0,            //PC to Station config
        PROBE_CONFIG=1,             //PC to stattion config
        SYSTEM_CONFIG=2,            //PC to station config
        SAVE_STATE=3,               //???
        MESSAGE=4,                  //outgoing->message of speficied type
        DATA=5,                     //outgoing-> readings and state data
        COMMAND=6,                  //incoming-> route to Controller
        ID_RECEIVE=7,               //Set station id
        ID_REQUEST=8,               //Request station id-Send to PC
        VER_RECIEVE=9,              //incoming-> new firmware version
        VER_REQUEST=10,             //incoming-> request version and send
        TEST_START_STATUS=11,       //outgoing->Notify PC that test has started
        TEST_COMPLETED=12,          //outgoing->Notify PC that test has completed
        TEST_LOAD_START=13,         //outgoing->Notify PC that test is starting from a load state
        HEATER_NOTIFY=14,           //Outgoing->Notify PC of a single heater tuning results
        HEATER_TUNE_COMPLETE=15,    //Outgoing->notfiy PC that all heaters have been tuned,
        ACK=16,                     //Outgoing->Acknowledge PC of command
        UPDATE_CURRENT=17,          //Incoming update current
        UPDATE_TEMP=18,             //Incoming update temperature
        TUNE_COM=19,                //Outgoing->tuning serial data,
        SEND_TEST_ID=20,            //Incoming->Start test
        LOAD_STATE=21,              //Incoming->Load saved state
    };

    const char strPre_00[] PROGMEM="CH";   //0
    const char strPre_01[] PROGMEM="CP";   //1
    const char strPre_02[] PROGMEM="CS";   //2
    const char strPre_03[] PROGMEM="ST";   //3
    const char strPre_04[] PROGMEM="M";    //4
    const char strPre_05[] PROGMEM="D";    //5
    const char strPre_06[] PROGMEM="COM";  //6
    const char strPre_07[] PROGMEM="IDREC";  //7
    const char strPre_08[] PROGMEM="IDREQ";   //8
    const char strPre_09[] PROGMEM="VERREC";  //9
    const char strPre_10[] PROGMEM="VERREQ";  //10
    const char strPre_11[] PROGMEM="TSTAT";     //11
    const char strPre_12[] PROGMEM="TCOMP";     //12
    const char strPre_13[] PROGMEM="TLOAD";     //13
    const char strPre_14[] PROGMEM="HNOTIFY";    //14
    const char strPre_15[] PROGMEM="HTUNED";     //15
    const char strPre_16[] PROGMEM="ACK";     //16
    const char strPre_17[] PROGMEM="UC";     //17
    const char strPre_18[] PROGMEM="UT";     //18
    const char strPre_19[] PROGMEM="TCOM";     //19
    const char strPre_20[] PROGMEM="TID";     //20
    const char strPre_21[] PROGMEM="LSTATE";     //21

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
        strPre_21
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
        strFile_03,
        strFile_04
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
        CURRENT_CHANGED=29
    };

    const char str_01[] PROGMEM=    "------Before Loading Free Memory: %d----------";  //0
    const char str_02[] PROGMEM=   "------Firmware Initialization Starting--------";  //1
    const char str_03[] PROGMEM=   "Loading configuration files...................";  //2
    const char str_04[] PROGMEM=    "After Loading Free Memory: %d.................";  //3
    const char str_05[] PROGMEM=   "Configuration Files Loaded....................";  //4
    const char str_06[] PROGMEM=    "--------Components Initalizing Starting-------";  //5
    const char str_07[] PROGMEM=   "Heaters initialized...........................";  //6
    const char str_08[] PROGMEM=   "Probes initialized............................";  //7
    const char str_09[] PROGMEM=   "Registering Components........................";  //8
    const char str_10[] PROGMEM=    "--------Components Initalized-----------------";  //9
    const char str_11[] PROGMEM=    "-------------Initalizing Timers---------------"; //10
    const char str_12[] PROGMEM=    "-------Timer Initialization Complete----------"; //11
    const char str_13[] PROGMEM=    "Check for saved state, Free Memory: %d";//12
    const char str_14[] PROGMEM=    "Saved state found, state loaded. Resuming from saved state";//13
    const char str_15[] PROGMEM=    "No saved state found, continuing to normal operation";//14
    const char str_16[] PROGMEM=    "------ Firmware Initialization Complete-------"; //15
    const char str_17[] PROGMEM=    "Tuning results saved"; //16
    const char str_18[] PROGMEM=    "Tuning canceled"; //17,
    const char str_19[] PROGMEM=    "Deleting saved state"; //18
    const char str_20[] PROGMEM=    "Saved state deleted"; //19
    const char str_21[] PROGMEM=    "Controller resetting, please wait......."; //20,
    const char str_22[] PROGMEM=    "SD Card Initialized"; //21
    const char str_23[] PROGMEM=    "TestController Transition from StateId %d from StateId %d"; //22
    const char str_24[] PROGMEM=    "Test Completed";//23
    const char str_25[] PROGMEM=    "Heater mode switched to auto tune";//23
    const char str_26[] PROGMEM=    "Heater mode switched to heating";//23
    const char str_27[] PROGMEM=    "TestController Transition from StateId %d from StateId %d"; //22
    const char str_28[] PROGMEM=    "Probe Test Started,Current: %d"; //23
    const char str_29[] PROGMEM=    "Probe Test Completed,Current Restored to %d"; //24
    const char str_30[] PROGMEM=    "Current Changed to %d"; //24

    const char* const system_message_table[] PROGMEM={
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
        TEST_ID_NOT_SET=27
    };

    const char strErr_01[] PROGMEM="Failed to load configuration files. Please contact administrator";
    const char strErr_02[] PROGMEM="Failed to load configuration file: %s. Defaults will be loaded. \n Please contact administarator";
    const char strErr_03[] PROGMEM="Failed to save configuration files.  Please contact administrator";
    const char strErr_04[] PROGMEM="Failed to save configuration file: %s. Changes will be lost on reset. Please contact administrator";
    const char strErr_05[] PROGMEM="SD card failed to initialize!! Defaults will be loaded. \n If you continue no changes to the configuration will be saved. Please check connections and contact administrator";
    const char strErr_06[] PROGMEM="Failed to save heater tuning results!! Please contact administrator";
    const char strErr_07[] PROGMEM="Failed to load saved state, station will continue to normal operation. Please contact administrator";
    const char strErr_08[] PROGMEM="Failed to delete saved state! Please contact administrator";
    const char strErr_09[] PROGMEM="Invalid command recieved! Please contact administrator";
    const char strErr_10[] PROGMEM="Failed to deserialize data: %s. Please contact administrator";
    const char strErr_11[] PROGMEM="Failed to serialize data: %s. Please contact administrator";
    const char strErr_12[] PROGMEM="Invalid message packet prefix recieved: %s";
    const char strErr_13[] PROGMEM="Prefix not found in message packet";
    const char strErr_14[] PROGMEM="Failed to transition to idle from running state, test is being hard stopped. \n restart controller before starting another test";
    const char strErr_15[] PROGMEM="Failed to start, test is already running";
    const char strErr_16[] PROGMEM="Failed to start, current or saveState was not set";
    const char strErr_17[] PROGMEM= "Failed to pause, test is already paused";
    const char strErr_18[] PROGMEM="Failed to continue, test is not paused";
    const char strErr_19[] PROGMEM="Failed to stop tuning, tuning is either in idle or complete state.";
    const char strErr_20[] PROGMEM="Failed to start tuning, tuning is running or complete state.";
    const char strErr_21[] PROGMEM="Failed to save tuning results, tuning is not in complete state";
    const char strErr_22[] PROGMEM="Failed to discard tuning, tuning is not in complete state";
    const char strErr_23[] PROGMEM="Error: Not in Tuning Mode, Switch modes to start tuning";
    const char strErr_24[] PROGMEM="Error: Not in Heating Mode, Switch modes to Turn On/Off Heaters";
    const char strErr_25[] PROGMEM="Error: Cannot change current or temperature while a test is running";
    const char strErr_26[] PROGMEM="Error: Temperature set point must be <= %d";
    const char strErr_27[] PROGMEM="Error: Cannot perform this action while a test is running";
    const char strErr_28[] PROGMEM="Error: Test Id must be set before starting a test";

    const char* const system_error_table[] PROGMEM={
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
        strErr_28
    };

#pragma endregion




// enum PacketType:uint8_t{
//     HEATER_CONFIG=0,
//     PROBE_CONFIG=1,
//     SYSTEM_CONFIG=2,
//     SAVE_STATE=3,
//     MESSAGE=4,
//     DATA=5,
//     COMMAND=6,
//     HEATER_RESPONSE=7, //Pc sends save response
//     TEST_RESPONSE=8,  //PC sends continue test request
//     HEATER_REQUEST=9,  //Pc recieves AutoTuneValues and request save response
//     TEST_REQUEST=10,   //Firmware sends continue test request
//     ID_RECEIVE=11,     //Set station id
//     ID_REQUEST=12,     //Request station id-Send to PC
//     VER_RECIEVE=13,
//     VER_REQUEST=14,
//     INIT=15,
//     TEST_START_STATUS=16, //Notify PC that test has started
//     TEST_COMPLETED=17,    //Notify PC that test has completed
//     TEST_LOAD_START=18,   //Notify PC that test is starting from a load state
// };

// const char* const prefixes[] PROGMEM = {
//     "CH",   //0
//     "CP",   //1
//     "CS",   //2
//     "ST",   //3
//     "M",    //4
//     "D",    //5
//     "COM",  //6
//     "HRES", //7
//     "TRES", //8
//     "HREQ", //9
//     "TREQ", //10
//     "IDREC",  //11
//     "IDREQ",   //12
//     "VERREC", //13
//     "VERREQ",  //14
//     "INIT",     //15
//     "TSTAT",     //16
//     "TCOMP",     //17
//     "TLOAD"      //18

// };

// const char* const system_error_table[] PROGMEM={
// /*0*/   "Failed to load configuration files. Please contact administrator",
// /*1*/   "Failed to load configuration file: %s. Defaults will be loaded. \n Please contact administarator",
// /*2*/   "Failed to save configuration files.  Please contact administrator",
// /*3*/   "Failed to save configuration file: %s. Changes will be lost on reset. Please contact administrator",
// /*4*/   "SD card failed to initialize!! Defaults will be loaded. \n If you continue no changes to the configuration will be saved. Please check connections and contact administrator",
// /*5*/   "Failed to save heater tuning results!! Please contact administrator",
// /*6*/   "Failed to load saved state, station will continue to normal operation. Please contact administrator",
// /*7*/   "Failed to delete saved state! Please contact administrator",
// /*8*/   "Invalid command recieved! Please contact administrator",
// /*9*/   "Failed to deserialize data: %s. Please contact administrator",
// /*10*/  "Failed to serialize data: %s. Please contact administrator",
// /*11*/  "Invalid message packet prefix recieved: %s."
// /*12*/  "Prefix not found in message packet",
// /*13*/  "Failed to transition to idle from running state, test is being hard stopped. \n restart controller before starting another test",
// /*14*/  "Failed to start, test is already running",
// /*15*/  "Failed to start, current or saveState was not set",
// /*16*/  "Failed to pause, test is already paused",
// /*17*/  "Failed to continue, test is not paused",
// };







