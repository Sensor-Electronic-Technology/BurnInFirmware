#pragma once
#include <Arduino.h>
#include "../SimpleFSM/SimpleFSM.h"
#include "../Logging/StationLogger.hpp"
#include "../Controller/BurnInTimer.hpp"



class TestController:public Component{
public:
    void Run();

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

private:
    BurnInTimer     timer;
    SimpleFSM       fsm;
    State states[5]={
        State("Running",[&](){this->RunningEnter();},[&](){this->Running();},[&](){this->RunningExit();}),
        State("Idle",[&](){this->IdleEnter();},[&](){this->Idle();},[&](){this->IdleExit();}),
        State("Paused",[&](){this->PausedEnter();},[&](){this->Paused();},[&](){this->PausedExit();}),
    }
};

