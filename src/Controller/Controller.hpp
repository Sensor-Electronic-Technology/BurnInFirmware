#pragma once
#include <ArduinoComponents.h>
#include "BurnInTimer.hpp"
#include "../constants.h"


class Controller:public Component{
public:
    void HandlerSerial();
    void Start(); 
    void Stop();
    void Pause();
    void RunProbeTest();
    void ToggleHeaters();
    void ChangeCurrent(CurrentValue current);
    void SendComs();

    //void invoke_action(ControllerAction action);
    // void transition_state(Task task);

    

private:
    //vector<BurnInTimer*> burnInTimers;

    typedef void(Controller::*StateHandlers)(void);
    StateHandlers state_handler[6];
    void privateLoop() override;
};