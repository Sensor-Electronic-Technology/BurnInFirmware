#pragma once
#include "../constants.h"
#include <LinkedList.h>
namespace State{



enum ModeState:uint8_t{
	INIT=0,
	HEATING=1,
	RUNNING=2,
	PAUSED=3,
	COMPLETED=4,
	IDLE=5,
	STATEERROR=6
};

enum StationMode:uint8_t{
	TUNE=0,
	NORMAL=1 //Default
};

// struct Transition{
// 	bool (*conditionCheck)();
// 	uint8_t stateNumber;
// 	Transition(bool(*cond)(),int stateNum){
// 		this->conditionCheck=cond;
// 		this->stateNumber=stateNum;
// 	}
// };

// class State{
// public:

// 	inline void addTransition(Transition* transition,State* state){
// 		Transition trans()
// 	}

// private:
// 	LinkedList<Transition*> transitions; 
// };

class Mode{
public:
	StationMode mode;
	bool continue_mode;
	bool latched;

	inline bool operator==(const Mode& rhs){
		return this->mode==rhs.mode;
	}

	inline bool operator!=(const Mode& rhs){
		return this->mode!=rhs.mode;
	}
};

class State{
public:
	ModeState state;
	bool continue_state;
	bool latched;

	inline bool operator==(const State& rhs){
		return this->state==rhs.state;
	}

	inline bool operator!=(const State& rhs){
		return this->state!=rhs.state;
	}
};

class Task{
public:
	Mode mode;
	State state;

};

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




};




