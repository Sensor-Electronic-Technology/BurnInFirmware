#include "TestController.hpp"

void TestController::SetFinsihedCallback(TestFinsihedCallback callback){
    this->_finishedCallback=callback;
}

void TestController::Tick(bool probesOkay[PROBE_COUNT]){
    this->burn_timer.Increment(probesOkay);

}

void TestController::SetCurrent(CurrentValue current){
    this->stressCurrent=current;
    this->currentSet=true;
}

void TestController::Reset(){
    this->burn_timer.Reset();
    this->currentSet=false;
    this->savedStateLoaded=false;
}//

const TimerData& TestController::GetTimerData(){
    return this->burn_timer.GetTimerData();
}

BurnInTimer* TestController::GetBurnTimer(){
    return &this->burn_timer;
}

bool TestController::StartTest(CurrentValue current){
    if(this->burn_timer.IsRunning()){
        ComHandler::SendErrorMessage(SystemError::START_ALREADY_RUNNING);
        return false;
    }
    this->currentSet=true;
    this->stressCurrent=current;
    this->burn_timer.Start(this->stressCurrent);
    ComHandler::SendStartResponse(true,F("Test Started"));
    this->ackTimer.onInterval([&](){
        ComHandler::SendStartResponse(true,F("Test Started"));
    },10000,true);
}

bool TestController::StartTest(const TimerData& savedState){
    this->savedState=savedState;
    this->savedStateLoaded=true;
    ComHandler::SendStartFromLoad(true,F("Test Started"));
    this->burn_timer.Start(this->savedState);
    this->ackTimer.onInterval([&](){
        ComHandler::SendStartFromLoad(true,F("Test Started"));
    },10000,true);
}

bool TestController::IsRunning(){
    return this->burn_timer.IsRunning();
}

bool TestController::PauseTest(){
    if(this->burn_timer.IsPaused()){
        ComHandler::SendErrorMessage(SystemError::PAUSE_ALREADY_PAUSED);
        return false;
    }
    this->burn_timer.Pause();
    return true;
}

bool TestController::ContinueTest(){
    if(!this->burn_timer.IsPaused()){
        ComHandler::SendErrorMessage(SystemError::CONTINUE_NOT_PAUSED);
        return false;
    }
    this->burn_timer.Continue();
}

