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

using namespace components;


/* enum StateId:uint8_t{
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
}; */


class TestController:public Component{
    typedef void(TestController::*StateRun)(void);
public:

    TestController(const BurnTimerConfig timerConfig):Component(),burn_timer(timerConfig){
        this->burn_timer.SetCallback([&](){
            ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_COMPLETED,MessageType::NOTIFY);
            this->Reset();
            this->_finishedCallback();
        });
    }

    TestController():Component(){
    }

    void SetConfig(const BurnTimerConfig timerConfig){
        this->burn_timer.SetConfig(timerConfig);
        this->burn_timer.SetCallback([&](){
            ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_COMPLETED,MessageType::NOTIFY);
            this->Reset();
            this->_finishedCallback();
        });
    }
    void Run();
    void SetFinsihedCallback(TestFinsihedCallback callback);
    void Tick(bool probesOkay[PROBE_COUNT]);

    //Start Test
    bool StartTest(CurrentValue current);
    bool StartTest(const TimerData& savedState);
    bool PauseTest();
    bool ContinueTest();
    bool IsRunning();

    void Reset();
    void SetCurrent(CurrentValue current);
    const TimerData& GetTimerData();
    BurnInTimer* GetBurnTimer();
    void BuildFSM();
private:
    void privateLoop() override{    }   

private:
    BurnInTimer            burn_timer;
    CurrentValue            stressCurrent;
    TimerData               savedState;
    bool                    currentSet=false;   
    bool                    savedStateLoaded=false;
    TestFinsihedCallback    _finishedCallback=[](){_NOP();}; 
    StateRun                stateRuns[STATE_COUNT];



};
