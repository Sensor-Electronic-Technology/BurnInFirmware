#pragma once
#include <Arduino.h>
#include "../StateMachine/StateMachine.hpp"
#include "../Probes/probe_constants.h"
#include "../TestTimer/burn_timer_includes.h"
#include "../Communication/ComHandler.hpp"
#include "../StationTimer.hpp"
#include "../constants.h"

#define STATE_COUNT         3
#define TRANSITION_COUNT    5
#define TEST_START_PERIOD   10000ul
#define TEST_COMP_PERIOD    10000ul


enum class TestState{
    TEST_IDLE=0,
    TEST_RUNNING=1,
    TEST_PAUSED=2
};

using namespace components;

class TestController:public Component{
    typedef void(TestController::*StateRun)(void);
public:

    TestController(const BurnTimerConfig timerConfig);

    TestController();

    void SetConfig(const BurnTimerConfig timerConfig);
    void Run();
    void SetFinsihedCallback(TestFinsihedCallback callback);
    void Tick(bool probesOkay[PROBE_COUNT]);
    void SetTestId(const char* id);
    void ClearTestId();

    //Start Test
    bool StartTest(CurrentValue current);
    bool StartTest(const TimerData& savedState,const char* id,CurrentValue current,int setTemp);
    bool PauseTest();
    bool ContinueTest();
    bool IsRunning();
    void SendTestStart();
    const char* GetTestId();
    void AcknowledgeTestStart();
    void AcknowledgeTestComplete();
    void GetProbeRunTimeOkay(bool *probeRtOkay);
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
    bool                    testIdSet=false;
    StationTimer            ackStartTimer,ackCompleteTimer;
    String                  testId;
    TestFinsihedCallback    _finishedCallback=[](){_NOP();};
    TestIdCallback         _testIdCallback=[](const char*){_NOP();};
    StateRun                stateRuns[STATE_COUNT];
};
