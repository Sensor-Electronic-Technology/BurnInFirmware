#pragma once
#include <Arduino.h>
#include "../StateMachine/StateMachine.hpp"
#include "../Probes/probe_constants.h"
#include "../TestTimer/burn_timer_includes.h"
#include "../Communication/ComHandler.hpp"
#include "../StationTimer.hpp"

#define STATE_COUNT         3
#define TRANSITION_COUNT    5

enum class TestState{
    TEST_IDLE=0,
    TEST_RUNNING=1,
    TEST_PAUSED=2
};

using namespace components;

class TestController:public Component{
    typedef void(TestController::*StateRun)(void);
public:

    TestController(const BurnTimerConfig timerConfig)
        :Component(),burn_timer(timerConfig){
        this->burn_timer.SetCallback([&](){
            ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_COMPLETED,MessageType::NOTIFY);
            this->Reset();
            this->_finishedCallback();
        });
        RegisterChild(ackTimer);
    }

    TestController():Component(){  }

    void SetConfig(const BurnTimerConfig timerConfig){
        this->burn_timer.SetConfig(timerConfig);
        this->burn_timer.SetCallback([&](){
            ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_COMPLETED,MessageType::NOTIFY);
            this->Reset();
            this->_finishedCallback();
        });
        RegisterChild(ackTimer);
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
    void SendTestStart();
    void AcknowledgeTestStart(){
        this->ackTimer.cancel();
    }

    void GetProbeRunTimeOkay(bool *probeRtOkay){
        this->burn_timer.GetProbeTimeOkay(probeRtOkay);
    }

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
    bool                    acknowledge=false;
    StationTimer            ackTimer;
    TestFinsihedCallback    _finishedCallback=[](){_NOP();}; 
    StateRun                stateRuns[STATE_COUNT];



};
