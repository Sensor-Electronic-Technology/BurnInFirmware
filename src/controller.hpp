#pragma once
#include <ArduinoComponents.h>
#include <ArduinoSTL.h>
#include "burn_timer.hpp"
#include "util.hpp"
#include "state.hpp"

using namespace components;
using namespace std;

class controller:public Component{
public:
    void handle_serial();
    void start(); 
    void stop();
    void pause();
    void probe_test();
    void toggle_heaters();
    void change_current(CurrentValue current);
    void send_coms();

    void invoke_action(ControllerAction action);
    // void transition_state(Task task);

    

private:
    vector<burn_timer*> burnInTimers;

    typedef void(controller::*StateHandlers)(void);
    StateHandlers state_handler[6];
};