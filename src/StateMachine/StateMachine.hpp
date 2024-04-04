#pragma once
#include <ArduinoComponents.h>

typedef components::Function<void(void)> TransitionActionHandler;
typedef components::Function<void(void)> StateActionHandler;
typedef components::Function<void(void*)> StateActionHandlerWithParam;
typedef components::Function<bool(void)> GuardConditionHandler;
typedef components::Function<void(void)> OnTransitionHandler;





class State{
    template<uint8_t N>friend class StateMachine;
public:
    State(){}
    State(uint8_t id):id(id){}
    State(uint8_t id,StateActionHandler action):id(id),action(action){}

    void StateRun(){
        this->action();
    }

    uint8_t GetId(){
        return this->id;
    }

    void SetAction(StateActionHandler action){
        this->action=action;
    }

private:
    StateActionHandler action=[](){};
    uint8_t id:4;
};

class Transition{
    template<uint8_t N>friend class StateMachine;
public:
    Transition(){}
    Transition(State* from,State* to,uint8_t transitionId):from(from),to(to){}
    Transition(State* from,State* to,uint8_t transitionId,uint8_t eventId,TransitionActionHandler action=[](){},GuardConditionHandler guard=[](){return true;})
            :from(from),to(to),transitionId(transitionId),eventId(eventId),action(action),guard(guard){}
    
    bool CanTransition(){
        return guard();
    }

    void OnTransition(){
        action();
    }

private:
    TransitionActionHandler action=[](){};
    GuardConditionHandler guard=[](){return true;};

    State*     to;
    State*     from;
    uint8_t    transitionId:4;
    uint8_t    eventId:4;
};

template<uint8_t N>
class StateMachine{
public:
    // StateMachine(State initial){}
    // StateMachine(State initial,transitions[N]):transitions(transitions){}
    StateMachine(){
        this->currentState=nullptr;
        this->previousState=nullptr;
        this->initialState=nullptr;

    }

    void Setup(State* initialState,const Transition (&transitions)[N]){
        this->currentState=initialState;
        this->initialState=initialState;
        for(uint8_t i=0;i<N;i++){
            this->transitions[i]=transitions[i];
        }
    }

    void SetOnTransitionHandler(OnTransitionHandler onTransition){
        this->onTransition=onTransition;
    }

    void Reset(){
        this->currentState=this->initialState;
        this->previousState=nullptr;
    }

    bool CanTransition(uint8_t eventId){
        for(uint8_t i=0;i<N;i++){
            if(transitions[i].eventId==eventId){
                if(this->currentState==transitions[i].from){
                    return transitions[i].CanTransition();
                }
            }
        }
        return false;
    }

    bool triggerEvent(uint8_t eventId){
        for(uint8_t i=0;i<N;i++){
            if(transitions[i].eventId==eventId){
                if(this->transitions[i].from==nullptr || this->currentState==transitions[i].from){
                    return this->_transition(&transitions[i]);
                }
            }
        }
        Serial.println("No Transition Found");
        return false;
    }

    void Run(){
        this->currentState->StateRun();
    }

    uint8_t GetCurrentStateId(){
        return this->currentState!=nullptr ? this->currentState->id : 99;
    }

    uint8_t GetPreviousStateId(){
        return this->previousState!=nullptr ? this->previousState->id : 99;
    }

private:
    Transition transitions[N];
    State* currentState=nullptr;
    State* previousState=nullptr;
    State* initialState=nullptr;
    OnTransitionHandler onTransition=[](){};

    bool _transition(Transition* transition){
        if(transition->CanTransition()){
            this->previousState=this->currentState;
            this->currentState=transition->to;
            this->onTransition();
            transition->OnTransition();
            return true;
        }
        return false;
    }
};