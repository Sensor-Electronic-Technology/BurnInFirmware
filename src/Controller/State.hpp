#pragma once
#include "constants.h"



enum ControllerState{
    RUNNING,
    PAUSED,
    IDLE,
    TEST_RUNNING
};

typedef struct SystemStatus{
    bool running=false;
    bool pasued=false;
	unsigned long elapsed=0;
}SystemStatus;

typedef struct SystemConfiguration{
    bool currentSwEnabled=false;

}SystemConfiguration;

typedef struct State{
	bool latched=false;
	bool continue_task=false;
	ControllerState controlState=IDLE;
	void clear(){
		this->latched=0;
        this->continue_task=0;
		this->controlState=IDLE;
	}
	bool operator==(const State &rhs)const{
		return this->controlState==rhs.controlState;
	}
	bool operator!=(const State &rhs)const{
		return this->controlState!=rhs.controlState;
	}
	State& operator=(const State &rhs){
		this->latched=rhs.latched;
		this->controlState=rhs.controlState;
        this->continue_task=rhs.continue_task;
		return *this;
	}
}State;







