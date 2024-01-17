#pragma once
#include "../constants.h"
#include <LinkedList.h>

enum HeaterStates:uint8_t{
	H_INIT=0,
	H_TUNING=1,
	H_IDLE=2,
	COMPLETED=3,
	H_ERROR=4
};

enum States:uint8_t{
	N_INIT=0,
	N_IDLE=1,
	N_PROBE_TEST=2,
	N_RUNNING=3,
	N_PAUSED=4,
	N_DONE=5,
	N_ERROR=6
};

enum CalStates:uint8_t{
	CALINIT=0,
	CALVOLTAGE=1,
	CALCURRENT=2,
	CALTEMP=3,
	CALDONE=4
};

enum StationMode:uint8_t{
	NORMAL=0, //Default
	TUNE=1,
	CAL=2
};

union ModeState {
	States n_state;
	CalStates c_state;
	HeaterStates h_states;
};


struct Task{
	StationMode mode;
	ModeState	state;
	bool s_latched=false;
	bool m_latched=false;

	void clear(){
		this->s_latched=false;
		this->s_latched=false;
	}

	bool operator==(const Task& rhs){
		return this->mode==rhs.mode;
	}

	bool operator!=(const Task& rhs){
		return this->mode!=rhs.mode;
	}
};






// struct Task{
// 	StationMode mode;
// 	ModeState state;

// 	bool operator==(const Task& rhs){
// 		return this->mode==rhs.mode;
// 	}

// 	bool operator!=(const Task& rhs){
// 		return this->mode!=rhs.mode;
// 	}

// };

// struct Transition{
// 	bool(*conditonFunc)();
// 	int stateIndex; 
// };

// class State{
// 	void addTransition(bool (*conditionFunc)(),State* s){

// 	}

// private:
// 	LinkedList<Transition*> transitions;	
// };

// class StateMachine{

// };









