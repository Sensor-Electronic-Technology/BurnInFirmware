#include "state.hpp"
#include "eeprom_wear_level.hpp"
#include "state.hpp"
#include "burn_timer.hpp"
#include <type_traits>
#include <ArduinoComponents.h>
#include <ArduinoSTL.h>

using namespace components;
using namespace std;

#define EEPROM_SIZE             50
#define ADDRESS_START           64
#define SYSTEM_SAVE_INT_SECS    60
#define TIMER_SAVE_INT_SECS     

struct timer_save_data{
    timer_data timers[7];
};

template<class T> class log:public Component{
public:
    log(int startAddr,unsigned long saveIntSec){
        T dat;
        this->wl=new eeprom_wear_level(dat,EEPROM_SIZE,startAddr,20);
        this->saveInterval=saveIntSec*1000;
    }
    
    bool checkExisting(){
        wl->begin();
        return wl->dataExist();
    }

    void start(T* dat=nullptr){
        if(dat!=nullptr){
            this->data=dat;
        }
        this->logTimer.onInterval([&](){
            this->save();
        },saveInterval);
    }

    int getEndAddress(){
        return this->wl->getEndAddress();
    }

    void read(){
        wl->get(*this->data);
    }

    void save(){
        wl->put(*this->data);
    }

private:
    T* data=nullptr;
    Timer logTimer;
    unsigned long saveInterval;
    eeprom_wear_level* wl;
    void privateLoop();
};

class log_controller:public Component{
public:
    void setup(){
        this->sysStateLog=new log<system_state>(StartAddr,SYSTEM_SAVE_INT_SECS);
        auto endAddr=this->sysStateLog->getEndAddress();
        this->timerLog=new log<timer_save_data>(endAddr+1,)
    }


private:
    void privateLoop();
    log<system_state>* sysStateLog;
    log<timer_save_data>*   timerLog; 

};