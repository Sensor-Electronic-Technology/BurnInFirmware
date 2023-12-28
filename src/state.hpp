#pragma once
#include "util.hpp"

enum ControllerAction{
    START,
    PAUSE,
    TOGGLE_HEAT,
    SWITCH_CURRENT,
    PROBE_TEST,
    UPDATE_CONFIG
};

enum ControllerState{
    RUNNING,
    PAUSED,
    IDLE,
    TEST_RUNNING
};

typedef struct system_state{
    bool running=false;
    bool pasued=false;
	unsigned long elapsed=0;
}system_state;

typedef struct system_configuration{
    bool currentSwEnabled=false;
    int temperatureSp=TEMP_DEFAULT;
    CurrentValue setCurrent=CURRENT_DEFAULT;
}system_configuration;

typedef struct state{
	bool latched=false;
	bool continue_task=false;
	ControllerState controlState=IDLE;
	void clear(){
		this->latched=0;
        this->continue_task=0;
		this->controlState=IDLE;
	}
	bool operator==(const state &rhs)const{
		return this->controlState==rhs.controlState;
	}
	bool operator!=(const state &rhs)const{
		return this->controlState!=rhs.controlState;
	}
	state& operator=(const state &rhs){
		this->latched=rhs.latched;
		this->controlState=rhs.controlState;
        this->continue_task=rhs.continue_task;
		return *this;
	}
}state;







