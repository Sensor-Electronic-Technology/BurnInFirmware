#pragma once
#include <Arduino.h>
#include "../StateMachine/StateMachine.hpp"
#include "../Probes/probe_constants.h"
#include "../TestTimer/burn_timer_includes.h"
#include "../Communication/ComHandler.hpp"

#define STATE_COUNT         3
#define TRANSITION_COUNT    5

enum class TestState{
    TEST_IDLE=0,
    TEST_RUNNING=1,
    TEST_PAUSED=2
};


enum StateId:uint8_t{
    TEST_IDLE=0,
    TEST_RUNNING=1,
    TEST_PAUSED=2
};

enum StateTrigger:uint8_t{
    STRESS_TEST_START=0,
    STRESS_TEST_PAUSE=1,
    STRESS_TEST_CONTINUE=2,
    STRESS_TEST_DONE=3,
    TEST_RESET=4
};

enum TransitionId:uint8_t{
    IDLE_TO_RUNNING=0,
    RUNNING_TO_PAUSED=1,
    PAUSED_TO_RUNNING=2,
    PAUSED_TO_IDLE=3,
    RUNNING_TO_IDLE=4
};


class TestController:public Component{
public:

    TestController(const BurnTimerConfig timerConfig):Component(),burn_timer(timerConfig){
        this->state_machine.Setup(&states[StateId::TEST_IDLE],this->transitions);
        this->state_machine.SetOnTransitionHandler([&](){
            auto from=this->state_machine.GetPreviousStateId();
            auto to=this->state_machine.GetCurrentStateId();
            ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_TRANSITION,MessageType::GENERAL,from,to);
        });
    }

    TestController(){
        this->state_machine.Setup(&states[StateId::TEST_IDLE],this->transitions);
        this->state_machine.SetOnTransitionHandler([&](){
            auto from=this->state_machine.GetPreviousStateId();
            auto to=this->state_machine.GetCurrentStateId();
            ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_TRANSITION,MessageType::GENERAL,from,to);
        });

    }

    void SetConfig(const BurnTimerConfig timerConfig){
        this->burn_timer.SetConfig(timerConfig);
    }

    void SetFinsihedCallback(TestFinsihedCallback callback);
    void Tick(bool probesOkay[PROBE_COUNT]);

    //Start Test
    bool StartTest(CurrentValue current);
    bool StartTest(const TimerData& savedState);
    bool CanStart();
    bool IsRunning();
    void Running();


    //Pause Test
    bool PauseTest();
    bool CanPause();

    //Continue Test
    bool ContinueTest();
    bool CanContinue();

    //Idle
    void OnIdleToRunning();
    void OnRunningToPaused();
    void OnRunningToIdle();
    void OnPausedToIdle();
    void OnPausedToRunning();

    void Reset();
    void SetCurrent(CurrentValue current);
    const TimerData& GetTimerData();
    BurnInTimer* GetBurnTimer();
    void BuildFSM();
private:
    void privateLoop() override{

    }

private:
    BurnInTimer            burn_timer;
    CurrentValue            stressCurrent;
    TimerData               savedState;
    bool                    currentSet=false;   
    bool                    savedStateLoaded=false;
    TestFinsihedCallback    _finishedCallback=[](){_NOP();}; 

    State states[STATE_COUNT]={
        State(StateId::TEST_IDLE),
        State(StateId::TEST_RUNNING,[&](){this->Running();}),
        State(StateId::TEST_PAUSED)
    };

    Transition transitions[TRANSITION_COUNT]={
        Transition(&states[StateId::TEST_IDLE],&states[StateId::TEST_RUNNING],
                TransitionId::IDLE_TO_RUNNING,
                StateTrigger::STRESS_TEST_START,
                [&](){this->OnIdleToRunning();},
                [&](){return this->CanStart();}),
        Transition(&states[StateId::TEST_RUNNING],&states[StateId::TEST_PAUSED],
                TransitionId::RUNNING_TO_PAUSED,
                StateTrigger::STRESS_TEST_PAUSE,
                [&](){this->OnRunningToPaused();},
                [&](){return this->CanPause();}),
        Transition(&states[StateId::TEST_PAUSED],&states[StateId::TEST_RUNNING],
                TransitionId::PAUSED_TO_RUNNING,
                StateTrigger::STRESS_TEST_CONTINUE,
                [&](){this->OnPausedToRunning();},
                [&](){return this->CanContinue();}),
        Transition(&states[StateId::TEST_RUNNING],&states[StateId::TEST_IDLE],
                TransitionId::RUNNING_TO_IDLE,
                StateTrigger::STRESS_TEST_DONE,
                [&](){this->OnRunningToIdle();}),
        Transition(nullptr,&states[StateId::TEST_IDLE],
                TransitionId::PAUSED_TO_IDLE,
                StateTrigger::TEST_RESET,
                [&](){this->OnPausedToIdle();})
    };        
    StateMachine<TRANSITION_COUNT> state_machine;
};

    // SimpleFSM state_machine;
    // State states[STATE_COUNT]={
    //     State(StateId::TEST_IDLE),
    //     State(StateId::TEST_RUNNING,[](){_NOP();},[&](){this->Running();},[](){_NOP();}),
    //     State(StateId::TEST_PAUSED),
    // };

    // Transition transitions[TRANSITION_COUNT]={
    //     Transition(&states[StateId::TEST_IDLE],&states[StateId::TEST_RUNNING],
    //             StateTrigger::STRESS_TEST_START,
    //             TransitionId::IDLE_TO_RUNNING,
    //             [&](){this->OnIdleToRunning();},
    //             [&](){return this->CanStart();}),
    //     Transition(&states[StateId::TEST_RUNNING],&states[StateId::TEST_PAUSED],
    //             (int)StateTrigger::STRESS_TEST_PAUSE,
    //             TransitionId::RUNNING_TO_PAUSED,
    //             [&](){this->OnRunningToPaused();},
    //             [&](){return this->CanPause();}),
    //     Transition(&states[StateId::TEST_PAUSED],&states[StateId::TEST_RUNNING],
    //             (int)StateTrigger::STRESS_TEST_CONTINUE,
    //             TransitionId::PAUSED_TO_RUNNING,
    //             [&](){this->OnPausedToRunning();},
    //             [&](){return this->CanContinue();}),
    //     Transition(&states[StateId::TEST_RUNNING],&states[StateId::TEST_IDLE],
    //             (int)StateTrigger::STRESS_TEST_DONE,
    //             TransitionId::RUNNING_TO_IDLE,
    //             [&](){this->OnRunningToIdle();},
    //             [](){return true;}),
    //     Transition(&states[StateId::TEST_PAUSED],&states[StateId::TEST_IDLE],
    //             (int)StateTrigger::TEST_RESET,
    //             TransitionId::PAUSED_TO_IDLE,
    //             [&](){this->OnPausedToIdle();},
    //             [](){return true;})
    // };


