#include "TestController.hpp"

void TestController::StartTest(){
    this->fsm.trigger(StateTrigger::STRESS_TEST_START);
}

void TestController::PauseTest(){
   
}

void TestController::ContinueTest(){
    
}

void TestController::BuildFSM(){

}



void TestController::Run(bool *probesOkay){

}

void TestController::IdleEnter(){
    this->burn_timer.Reset();
    // this->timer.Reset();
    // this->timer.SetTime(this->config.burnTimerConfig.time60mASecs);
    // this->timer.Start();
}

void TestController::Idle(){
    // if(this->timer.IsDone()){
    //     this->fsm.TransitionTo("Running");
    // }
}


void TestController::RunningEnter(){
    this->burn_timer.Start();
}

void TestController::Running(){
    if(this->burn_timer.IsDone()){
        this->fsm.trigger(StateTrigger::STRESS);
    }
}

void TestController::RunningExit(){
    // this->timer.Start();

    // this->timer.Stop();
}

void TestController::PausedEnter(){
    // this->timer.Stop();
}

void TestController::Paused(){
    //Do nothing
}

void TestController::PausedExit(){
    // this->timer.Start();
}

void TestController::privateLoop(){
    //this->timer.Update();
}



