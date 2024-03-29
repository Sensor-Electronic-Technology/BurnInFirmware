#pragma once
#include <Arduino.h>
#include "../SimpleFSM/SimpleFSM.h"
#include "../Logging/StationLogger.hpp"
#include "../Probes/probe_constants.h"
#include "../TestTimer/burn_timer_includes.h"

#define STATE_COUNT     3
#define TRANSITION_COUNT 4

enum StateId{
    IDLE=0,
    RUNNING=1,
    PAUSED=2
};

enum StateTrigger{
    START=0,
    PAUSE=1,
    CONTINUE=2,
    RESET=3
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
    void StartTest(CurrentValue currentValue);
    void PauseTest();
    void ContinueTest();



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
    BurnInTimer     timer;
    SimpleFSM       fsm;
    State<StateId> states[STATE_COUNT]={
        State<StateId>(StateId::IDLE,[&](){this->IdleEnter();},[&](){this->Idle();},[&](){this->IdleExit();}),
        State<StateId>(StateId::RUNNING,[&](){this->RunningEnter();},[&](){this->Running();},[&](){this->RunningExit();}),
        State<StateId>(StateId::PAUSED,[&](){this->PausedEnter();},[&](){this->Paused();},[&](){this->PausedExit();}),
    };

    Transition transitions[TRANSITION_COUNT]={
        Transition(&states[StateId::IDLE],&states[StateId::RUNNING],StateTrigger::START),
        Transition(&states[StateId::RUNNING],&states[StateId::PAUSED],StateTrigger::PAUSE),
        Transition(&states[StateId::PAUSED],&states[StateId::RUNNING],StateTrigger::CONTINUE),
        Transition(&states[StateId::PAUSED],&states[StateId::IDLE],StateTrigger::RESET)
        
    };

};

