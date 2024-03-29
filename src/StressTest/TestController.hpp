#pragma once
#include <Arduino.h>
#include "../SimpleFSM/SimpleFSM.h"
#include "../Logging/StationLogger.hpp"
#include "../Probes/probe_constants.h"
#include "../TestTimer/burn_timer_includes.h"

#define STATE_COUNT     3
#define TRANSITION_COUNT 4

enum StateId{
    TEST_IDLE=0,
    TEST_RUNNING=1,
    TEST_PAUSED=2
};

enum StateTrigger{
    STRESS_TEST_START=0,
    STRESS_TEST_PAUSE=1,
    STRESS_TEST_CONTINUE=2,
    STRESS_TEST_DONE=3,
    TEST_RESET=4
};

enum TransitionId{
    IDLE_TO_RUNNING=0,
    RUNNING_TO_PAUSED=1,
    PAUSED_TO_RUNNING=2,
    PAUSED_TO_IDLE=3,
    RUNNING_TO_IDLE=4
};


class TestController:public Component{
public:
    void Run(bool *probesOkay);
    void StartTest();
    void SetCurrent(CurrentValue current){
        this->stressCurrent=current;
        this->currentSet=true;
    }
    bool CanStart(){
        return this->burn_timer.IsDone() && this->currentSet;
    }   
    void PauseTest();
    bool CanPause(){
        return this->fsm.getState()->getStateId()==StateId::TEST_RUNNING;
    }
    void ContinueTest();
    bool CanContinue(){
        return this->fsm.getState()->getStateId()==StateId::TEST_PAUSED;
    }



private:
    void BuildFSM();
    //Idle
    void IdleEnter();
    void Idle();
    void IdleExit();

    //Running
    void RunningEnter();
    void Running();
    void RunningExit();

    //Paused
    void PausedEnter();
    void Paused();
    void PausedExit();

    void privateLoop() override;

private:
    BurnInTimer                             burn_timer;
    CurrentValue                            stressCurrent;
    bool                                    currentSet=false;    
    SimpleFSM<StateId,TransitionId>         fsm;
    State<StateId> states[STATE_COUNT]={
        State<StateId>(StateId::TEST_IDLE,[&](){this->IdleEnter();},[&](){this->Idle();},[&](){this->IdleExit();}),
        State<StateId>(StateId::TEST_RUNNING,[&](){this->RunningEnter();},[&](){this->Running();},[&](){this->RunningExit();}),
        State<StateId>(StateId::TEST_PAUSED,[&](){this->PausedEnter();},[&](){this->Paused();},[&](){this->PausedExit();}),
    };

    Transition<StateId,TransitionId> transitions[TRANSITION_COUNT]={
        Transition<StateId,TransitionId>(&states[StateId::TEST_IDLE],&states[StateId::TEST_RUNNING],StateTrigger::STRESS_TEST_START,TransitionId::IDLE_TO_RUNNING,[&](){return this->CanStart();}),
        Transition<StateId,TransitionId>(&states[StateId::TEST_RUNNING],&states[StateId::TEST_PAUSED],StateTrigger::STRESS_TEST_PAUSE,TransitionId::RUNNING_TO_PAUSED,[&](){return true;}),
        Transition<StateId,TransitionId>(&states[StateId::TEST_PAUSED],&states[StateId::TEST_RUNNING],StateTrigger::STRESS_TEST_CONTINUE,TransitionId::PAUSED_TO_RUNNING,[&](){return true;},
        Transition<StateId,TransitionId>(&states[StateId::TEST_RUNNING],&states[StateId::TEST_IDLE],StateTrigger::STRESS_TEST_DONE,TransitionId::PAUSED_TO_RUNNING,[&](){return true;},
        Transition<StateId,TransitionId>(&states[StateId::TEST_PAUSED],&states[StateId::TEST_IDLE],StateTrigger::TEST_RESET,TransitionId::PAUSED_TO_IDLE,[&](){return true;})
    };
};

