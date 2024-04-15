#pragma once
#include "constants.h"
#include "Communication/ComHandler.hpp"

template <typename T>
struct Task{
    T state,nextState;

    void set(T _state,T _nextState){
        this->state=_state;
        this->nextState=_nextState;
    }

    void transition(){
        ComHandler::SendSystemMessage(SystemMessage::HEATER_STATE_TRANSITION,MessageType::GENERAL,this->state,this->nextState);
        this->state=this->nextState;
    }

    bool is_next(T _state){
        return this->nextState==_state;
    }

    explicit operator bool(){
        return this->nextState!=this->state;
    }

    bool operator==(T _state){
        return this->state==_state;
    }

    bool operator!=(T _state){
        return this->state!=_state;
    }
}; 