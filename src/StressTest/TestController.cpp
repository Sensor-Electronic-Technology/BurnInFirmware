#include "TestController.hpp"

void TestController::StartTest(CurrentValue currentValue){
    this->fsm.trigger(StateTrigger::START);
}

void TestController::PauseTest(){
   
}

void TestController::ContinueTest(){
    
}

void TestController::BuildFSM(){

}



void TestController::Run(bool *probesOkay){

}

void TestController::privateLoop(){
    //this->timer.Update();
}

void TestController::IdleEnter(){
    // this->timer.Reset();
    // this->timer.SetTime(this->config.burnTimerConfig.time60mASecs);
    // this->timer.Start();
}

void TestController::Idle(){
    // if(this->timer.IsDone()){
    //     this->fsm.TransitionTo("Running");
    // }
}

void TestController::IdleExit(){
    //this->timer.Stop();
}

void TestController::RunningEnter(){
    // this->timer.Reset();
    // this->timer.SetTime(this->config.burnTimerConfig.time120mASecs);
    // this->timer.Start();
}

void TestController::Running(){
    // if(this->timer.IsDone()){
    //     this->fsm.TransitionTo("Idle");
    // }
}

void TestController::RunningExit(){
    this->timer.Stop();
}

void TestController::PausedEnter(){
    this->timer.Stop();
}

void TestController::Paused(){
    //Do nothing
}

void TestController::PausedExit(){
    this->timer.Start();
}



