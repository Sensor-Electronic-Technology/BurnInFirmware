#include "TestController.hpp"

    void TestController::SetFinsihedCallback(TestFinsihedCallback callback){
        this->_finishedCallback=callback;
    }

    void TestController::Tick(bool probesOkay[PROBE_COUNT]){
        if(this->currentState!=this->nextState){
            switch(this->nextState){
                case TestState::TEST_IDLE:{
                    if(this->currentState==TestState::TEST_RUNNING){
                        this->OnRunningToIdle();
                    }else if(this->currentState==TestState::TEST_PAUSED){
                        this->OnPausedToIdle();
                    }
                    break;
                }
                case TestState::TEST_RUNNING:{
                    if(this->currentState==TestState::TEST_IDLE){
                        this->OnIdleToRunning();
                    }else if(this->currentState==TestState::TEST_PAUSED){
                        this->OnPausedToRunning();
                    }
                    break;
                }
                case TestState::TEST_PAUSED:{
                    if(this->currentState==TestState::TEST_RUNNING){
                        this->OnRunningToPaused();
                    }
                    break;
                }
            }
            this->currentState=this->nextState;
        }
        if(this->currentState==TestState::TEST_RUNNING){
            this->Running(probesOkay);
        }
        //this->burn_timer.Increment(probesOkay);
        // this->state_machine.run(500);
    }

    void TestController::SetCurrent(CurrentValue current){
        this->stressCurrent=current;
        this->currentSet=true;
    }

    void TestController::BuildFSM(){
        // this->state_machine.add(transitions,TRANSITION_COUNT);
        // this->state_machine.setInitialState(&states[StateId::TEST_IDLE]);
        // this->state_machine.setTransitionHandler([&](){
        //     auto from=this->state_machine.getPreviousState()->getStateId();
        //     auto to=this->state_machine.getState()->getStateId();
        //     StationLogger::Log(LogLevel::INFO,true,false,F("Test Controller Transition from StateId %d from StateId %d"),(int)from,(int)to);
        // });
    }

    void TestController::Reset(){
        //this->state_machine.trigger(StateTrigger::TEST_RESET);
        this->burn_timer.Reset();
        this->nextState=TestState::TEST_IDLE;
        this->currentSet-false;
        this->savedStateLoaded=false;
    }//

    const TimerData& TestController::GetTimerData(){
        return this->burn_timer.GetTimerData();
    }

    BurnInTimer* TestController::GetBurnTimer(){
        return &this->burn_timer;
    }

    void TestController::Running(bool probesOkay[PROBE_COUNT]){
        this->burn_timer.Increment(probesOkay);
        if(this->burn_timer.IsDone()){
            this->_finishedCallback();
            this->nextState=TestState::TEST_IDLE;
        }
        // if(this->burn_timer.IsDone()){
        //     // if(!this->state_machine.trigger(StateTrigger::STRESS_TEST_DONE)){
        //     //     StationLogger::Log(LogLevel::CRITICAL_ERROR,true,false,F("Failed to transition to idle from running state, test is being hard stopped. \n restart controller before starting another test"));
        //     //     this->_finishedCallback();
        //     // }
        // }
    }

    //Start Test
#pragma region StartTest
    bool TestController::StartTest(CurrentValue current){
        this->currentSet=true;
        this->stressCurrent=current;

        if(this->burn_timer.IsRunning()){
            ComHandler::SendStartResponse(false,F("Failed to start, test is already running"));
            //StationLogger::Log(LogLevel::ERROR,true,false,F("Failed to start, test is already running"));
            return false;
        }
        ComHandler::SendStartResponse(true,F("Test Started"));
        //StationLogger::Log(LogLevel::INFO,true,false,F("Test Started"));
        this->nextState=TestState::TEST_RUNNING;
        return true;
    }

    bool TestController::StartTest(const TimerData& savedState){
        this->savedState=savedState;
        this->savedStateLoaded=true;

        if(this->burn_timer.IsRunning()){
            ComHandler::SendStartResponse(false,F("Failed to start, test is already running"));
            //StationLogger::Log(LogLevel::ERROR,true,false,F("Failed to start, test is already running"));
            return false;
        }

        ComHandler::SendStartResponse(true,F("Test Started"));
        this->nextState=TestState::TEST_RUNNING;
        return true;
    }

    bool TestController::CanStart(){
        return !this->burn_timer.IsRunning() && (this->currentSet || this->savedStateLoaded);
    }

    bool TestController::IsRunning(){
        return this->burn_timer.IsRunning();
    }
#pragma endregion

#pragma region PauseTest
    bool TestController::PauseTest(){
        if(!this->burn_timer.IsRunning()){
            StationLogger::Log(LogLevel::WARNING,true,false,F("Failed to pause, test is not running"));
            return false;
        }
        this->nextState=TestState::TEST_PAUSED;
        return true;
    }

    bool TestController::CanPause(){
        return !this->burn_timer.IsPaused();
    }
#pragma endregion

#pragma region ContinueTest
    bool TestController::ContinueTest(){
        if(!this->burn_timer.IsPaused()){
            StationLogger::Log(LogLevel::WARNING,true,false,F("Failed to continue, test is not pasued"));
            return false;
        }

        this->nextState=TestState::TEST_RUNNING;
        return true;
    }

    bool TestController::CanContinue(){
        return this->burn_timer.IsPaused();
    }
#pragma endregion

#pragma region TransitionHandlers

    void TestController::OnRunningToIdle(){
        this->Reset();
        this->_finishedCallback();
    }

    void TestController::OnIdleToRunning(){

        if(this->savedStateLoaded){
            this->burn_timer.Start(this->savedState);
            return;
        }

        if(this->currentSet){
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