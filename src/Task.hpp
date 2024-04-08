#pragma once

template <typename T>
struct Task{
    T state,nextState;

    void set(T _state,T _nextState){
        this->state=_state;
        this->nextState=_nextState;
    }

    void transition(){
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