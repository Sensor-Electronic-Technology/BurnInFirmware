#include "TestController.hpp"

    void TestController::SetFinsihedCallback(TestFinsihedCallback callback){
        this->_finishedCallback=callback;
    }

    void TestController::Tick(bool *probesOkay){
        this->burn_timer->Increment(probesOkay);
        this->state_machine.run(0);
    }

    void TestController::SetCurrent(CurrentValue current){
        this->stressCurrent=current;
        this->currentSet=true;
    }

    void TestController::BuildFSM(){
        this->state_machine.add(transitions,TRANSITION_COUNT);
        this->state_machine.setInitialState(&states[StateId::TEST_IDLE]);
        this->state_machine.setTransitionHandler([&](){
            auto from=this->state_machine.getPreviousState()->getStateId();
            auto to=this->state_machine.getState()->getStateId();
            StationLogger::Log(LogLevel::INFO,true,false,F("Test Controller Transition from StateId %d from StateId %d"),(int)from,(int)to);
        });
    }

    void TestController::Reset(){
        this->state_machine.trigger(StateTrigger::TEST_RESET);
    }//

    const TimerData& TestController::GetTimerData(){
        return this->burn_timer->GetTimerData();
    }

    BurnInTimer* TestController::GetBurnTimer(){
        return this->burn_timer;
    }

    void TestController::Running(){
        if(this->burn_timer->IsDone()){
            if(!this->state_machine.trigger(StateTrigger::STRESS_TEST_DONE)){
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
        if(this->burn_timer->IsRunning()){
            ComHandler::SendStartResponse(false,F("Failed to start, test is already running"));
            StationLogger::Log(LogLevel::ERROR,true,false,F("Failed to start, test is already running"));
            return false;
        }

        if(this->state_machine.trigger(StateTrigger::STRESS_TEST_START)){
            ComHandler::SendStartResponse(true,F("Test Started"));
            StationLogger::Log(LogLevel::INFO,true,false,F("Test Started"));
            return true;
        }else{
            ComHandler::SendStartResponse(false,F("Failed to start, internal state machine error"));
            StationLogger::Log(LogLevel::ERROR,true,false,F("Failed to start, internal state machine error"));
            return false;
        }
    }

    bool TestController::StartTest(const TimerData& savedState){
        this->savedState=savedState;
        this->savedStateLoaded=true;
        if(this->state_machine.trigger((int)StateTrigger::STRESS_TEST_START)){
            ComHandler::SendStartFromLoad(true,F("Test Started, starting from saved state"));
            return true;
        }else{
            ComHandler::SendStartFromLoad(false,F("Failed to start, internal state machine error"));
            return false;
        }
    }

    bool TestController::CanStart(){
        return !this->burn_timer->IsRunning() && (this->currentSet || this->savedStateLoaded);
    }

    bool TestController::IsRunning(){
        return this->burn_timer->IsRunning();
    }
#pragma endregion

#pragma region PauseTest
    bool TestController::PauseTest(){
        return this->state_machine.trigger((int)StateTrigger::STRESS_TEST_PAUSE);
    }

    bool TestController::CanPause(){
        return !this->burn_timer->IsPaused();
    }
#pragma endregion

#pragma region ContinueTest
    bool TestController::ContinueTest(){
        return this->state_machine.trigger((int)StateTrigger::STRESS_TEST_CONTINUE);
    }

    bool TestController::CanContinue(){
        return this->burn_timer->IsPaused();
    }
#pragma endregion

#pragma region TransitionHandlers

    void TestController::OnRunningToIdle(){
        this->burn_timer->Reset();
        this->_finishedCallback();
    }

    void TestController::OnIdleToRunning(){
        if(this->savedStateLoaded){
            this->burn_timer->Start(this->savedState);
            return;
        }

        if(this->currentSet){
            this->burn_timer->Start(this->stressCurrent);
            return;
        }
    }

    void TestController::OnRunningToPaused(){
        this->burn_timer->Pause();
    }

    void TestController::OnPausedToIdle(){
        this->burn_timer->Reset();
    }

    void TestController::OnPausedToRunning(){
        this->burn_timer->Continue();
    }

#pragma endregion