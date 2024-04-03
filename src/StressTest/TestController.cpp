#include "TestController.hpp"

    void TestController::SetFinsihedCallback(TestFinsihedCallback callback){
        this->_finishedCallback=callback;
    }

    // void TestController::Tick(bool probesOkay[PROBE_COUNT]){
    //     if(this->currentState!=this->nextState){
    //         switch(this->nextState){
    //             case TestState::TEST_IDLE:{
    //                 if(this->currentState==TestState::TEST_RUNNING){
    //                     this->OnRunningToIdle();
    //                 }else if(this->currentState==TestState::TEST_PAUSED){
    //                     this->OnPausedToIdle();
    //                 }
    //                 break;
    //             }
    //             case TestState::TEST_RUNNING:{
    //                 if(this->currentState==TestState::TEST_IDLE){
    //                     this->OnIdleToRunning();
    //                 }else if(this->currentState==TestState::TEST_PAUSED){
    //                     this->OnPausedToRunning();
    //                 }
    //                 break;
    //             }
    //             case TestState::TEST_PAUSED:{
    //                 if(this->currentState==TestState::TEST_RUNNING){
    //                     this->OnRunningToPaused();
    //                 }
    //                 break;
    //             }
    //         }
    //         this->currentState=this->nextState;
    //     }
    //     if(this->currentState==TestState::TEST_RUNNING){
    //         this->Running(probesOkay);
    //     }
    //     //this->burn_timer.Increment(probesOkay);
    //     // this->state_machine.run(500);
    // }

    void TestController::Tick(bool probesOkay[PROBE_COUNT]){
        this->burn_timer.Increment(probesOkay);
    }

    void TestController::SetCurrent(CurrentValue current){
        this->stressCurrent=current;
        this->currentSet=true;
    }

    void TestController::Reset(){
        if(this->state_machine.triggerEvent(StateTrigger::TEST_RESET)){
            this->burn_timer.Reset();
            this->currentSet-false;
            this->savedStateLoaded=false;
        }
    }//

    const TimerData& TestController::GetTimerData(){
        return this->burn_timer.GetTimerData();
    }

    BurnInTimer* TestController::GetBurnTimer(){
        return &this->burn_timer;
    }

    void TestController::Running(){
        if(this->burn_timer.IsDone()){
            if(!this->state_machine.triggerEvent(StateTrigger::STRESS_TEST_DONE)){
                StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Failed to transition to idle from running state, test is being hard stopped. \n restart controller before starting another test"));
                this->_finishedCallback();
            }
        }
    }

    //Start Test
#pragma region StartTest
    bool TestController::StartTest(CurrentValue current){
        this->currentSet=true;
        this->stressCurrent=current;
        return this->state_machine.triggerEvent(StateTrigger::STRESS_TEST_START);
    }

    bool TestController::StartTest(const TimerData& savedState){
        this->savedState=savedState;
        this->savedStateLoaded=true;
        return this->state_machine.triggerEvent(StateTrigger::STRESS_TEST_START);
    }

    bool TestController::CanStart(){
        if(this->burn_timer.IsRunning()){
            StationLogger::Log(LogLevel::WARNING,true,false,F("Failed to start, test is already running"));
            return false;
        }
        
        if(!this->currentSet && !this->savedStateLoaded){
            StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Failed to start, current or saveState was not set"));
            return false;
        }
        //return !this->burn_timer.IsRunning() && (this->currentSet || this->savedStateLoaded);
        return true;
    }

    bool TestController::IsRunning(){
        return this->burn_timer.IsRunning();
    }
#pragma endregion

#pragma region PauseTest
    bool TestController::PauseTest(){
        return this->state_machine.triggerEvent(StateTrigger::STRESS_TEST_PAUSE);
    }

    bool TestController::CanPause(){
        if(this->burn_timer.IsPaused()){
            StationLogger::Log(LogLevel::WARNING,true,false,F("Failed to pause, test is already paused"));
            return false;
        }
        return true;
    }
#pragma endregion

#pragma region ContinueTest
    bool TestController::ContinueTest(){
        return this->state_machine.triggerEvent(StateTrigger::STRESS_TEST_CONTINUE);
    }

    bool TestController::CanContinue(){
        if(!this->burn_timer.IsPaused()){
            StationLogger::Log(LogLevel::WARNING,true,false,F("Failed to continue, test is not paused"));
            return false;
        }
        return true;
    }
#pragma endregion

#pragma region TransitionHandlers

    void TestController::OnRunningToIdle(){
        this->Reset();
        this->_finishedCallback();
    }

    void TestController::OnIdleToRunning(){

        if(this->savedStateLoaded){
            ComHandler::SendStartResponse(true,F("Test Started"));
            this->burn_timer.Start(this->savedState);
            return;
        }

        if(this->currentSet){
            ComHandler::SendStartResponse(true,F("Test Started"));
            this->burn_timer.Start(this->stressCurrent);
            return;
        }
    }

    void TestController::OnRunningToPaused(){
        this->burn_timer.Pause();
    }

    void TestController::OnPausedToIdle(){
        this->burn_timer.Reset();
    }

    void TestController::OnPausedToRunning(){
        this->burn_timer.Continue();
    }

#pragma endregion