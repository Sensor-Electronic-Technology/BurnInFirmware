#pragma once
#include <Arduino.h>
#include "../SimpleFSM/SimpleFSM.h"
#include "../Logging/StationLogger.hpp"
#include "../Probes/probe_constants.h"
#include "../TestTimer/burn_timer_includes.h"

#define STATE_COUNT     3
#define TRANSITION_COUNT 5

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

    TestController(const BurnTimerConfig timerConfig):Component(){
        this->burn_timer=new BurnInTimer(timerConfig);
        this->BuildFSM();
    }

    void SetFinsihedCallback(TestFinsihedCallback callback){
        this->_finishedCallback=callback;
    }

    void Tick(bool *probesOkay){
        this->burn_timer->Increment(probesOkay);
        this->state_machine.run(0,NULL);
    }

    void StartTest(){
        this->state_machine.trigger(StateTrigger::STRESS_TEST_START);
    }

    void StartTestFrom(CurrentValue current,const TimerData& timerData){
        this->stressCurrent=current;
        this->savedState=timerData;
        this->currentSet=true;
        this->savedStateLoaded=true;
    }

    void SetCurrent(CurrentValue current){
        this->stressCurrent=current;
        this->currentSet=true;
    }

    const TimerData& GetTimerData(){
        return this->burn_timer->GetTimerData();
    }

    BurnInTimer* GetBurnTimer(){
        return this->burn_timer;
    }

    bool IsRunning(){
        return (this->state_machine.getState()->getStateId()==StateId::TEST_RUNNING
                || this->state_machine.getState()->getStateId()==StateId::TEST_PAUSED)
                && this->burn_timer->IsRunning();
    }

    bool CanStart(){
        return !this->burn_timer->IsRunning() && this->currentSet;
    }   

    void PauseTest(){
        this->state_machine.trigger(StateTrigger::STRESS_TEST_PAUSE);
    }

    bool CanPause(){
        return !this->burn_timer->IsPaused();
    }

    void ContinueTest(){
        this->state_machine.trigger(StateTrigger::STRESS_TEST_CONTINUE);
    }

    bool CanContinue(){
        return this->burn_timer->IsPaused();
    }

    void Reset(){
        this->state_machine.trigger(StateTrigger::TEST_RESET);
    }

private:
    void BuildFSM(){
        this->state_machine.add(transitions,TRANSITION_COUNT);
        this->state_machine.setInitialState(&states[StateId::TEST_IDLE]);
        this->state_machine.setTransitionHandler([&](){
            auto from=this->state_machine.getPreviousState()->getStateId();
            auto to=this->state_machine.getState()->getStateId();
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Test Controller Transition from StateId %d from StateId %d"),(int)from,(int)to);
        });
        this->state_machine.setInitialState(&this->states[StateId::TEST_IDLE]);
    }
    //Idle
    void IdleEnter(){
        this->burn_timer->Reset();
    }
    void Idle(){
        _NOP();
    }
    void IdleExit(){
        _NOP();
    }

    //Running
    void RunningEnter(){
        this->burn_timer->Start();
    }
    void Running();
    void RunningExit(){
        this->burn_timer->Stop();
        this->_finishedCallback();
    }

    //Paused
    void PausedEnter();
    void Paused();
    void PausedExit();

    void privateLoop() override;

private:
    BurnInTimer*            burn_timer;
    CurrentValue            stressCurrent;
    TimerData               savedState;
    bool                    currentSet=false;   
    bool                    savedStateLoaded=false;
    TestFinsihedCallback    _finishedCallback=[](){_NOP();}; 
    SimpleFSM<StateId,TransitionId> state_machine;
    State<StateId> states[STATE_COUNT]={
        State<StateId>(StateId::TEST_IDLE,[&](){this->IdleEnter();},[&](){this->Idle();},[&](){this->IdleExit();}),
        State<StateId>(StateId::TEST_RUNNING,[&](){this->RunningEnter();},[&](){this->Running();},[&](){this->RunningExit();}),
        State<StateId>(StateId::TEST_PAUSED,[&](){this->PausedEnter();},[&](){this->Paused();},[&](){this->PausedExit();}),
    };

    Transition<StateId,TransitionId> transitions[TRANSITION_COUNT]={
        Transition<StateId,TransitionId>(&states[StateId::TEST_IDLE],&states[StateId::TEST_RUNNING],
                StateTrigger::STRESS_TEST_START,
                TransitionId::IDLE_TO_RUNNING,
                [&](){return this->CanStart();}),
        Transition<StateId,TransitionId>(&states[StateId::TEST_RUNNING],&states[StateId::TEST_PAUSED],
                StateTrigger::STRESS_TEST_PAUSE,
                TransitionId::RUNNING_TO_PAUSED,
                [&](){return this->CanPause();}),
        Transition<StateId,TransitionId>(&states[StateId::TEST_PAUSED],&states[StateId::TEST_RUNNING],
                StateTrigger::STRESS_TEST_CONTINUE,
                TransitionId::PAUSED_TO_RUNNING,
                [&](){return this->CanContinue();}),
        Transition<StateId,TransitionId>(&states[StateId::TEST_RUNNING],&states[StateId::TEST_IDLE],
                StateTrigger::STRESS_TEST_DONE,
                TransitionId::RUNNING_TO_IDLE,
                [&](){return true;}),
        Transition<StateId,TransitionId>(&states[StateId::TEST_PAUSED],&states[StateId::TEST_IDLE],
                StateTrigger::TEST_RESET,
                TransitionId::PAUSED_TO_IDLE,
                [&](){return true;})
    };
};

