#include "TestController.hpp"

TestController::TestController(const BurnTimerConfig timerConfig):Component(),burn_timer(timerConfig){
    this->burn_timer.SetCallback([&](){
        this->CompleteTest();
    });
    this->_testIdCallback=[&](const char* id){
        this->SetTestId(id);
    };
    this->ackStartTimer.onInterval([&](){
        ComHandler::SendStartResponse(true,F("Test Started"));
    },TEST_START_PERIOD,false,true);

    this->ackCompleteTimer.onInterval([&](){
        ComHandler::SendTestCompleted(F("Test Completed"));
    },TEST_COMP_PERIOD,false,true);

    ComHandler::MapTestIdCallback(this->_testIdCallback);
    this->testId.reserve(TEST_ID_LENGTH);
    this->ClearTestId();
    RegisterChild(ackStartTimer);
    RegisterChild(ackCompleteTimer);
}

TestController::TestController():Component(){  
    this->burn_timer.SetCallback([&](){
        this->CompleteTest();
    });

    this->_testIdCallback=[&](const char* id){
        this->SetTestId(id);
    };

    this->ackStartTimer.onInterval([&](){
        ComHandler::SendStartResponse(true,F("Test Started"));
    },TEST_START_PERIOD,false,true);

    this->ackCompleteTimer.onInterval([&](){
        ComHandler::SendTestCompleted(F("Test Completed"));
    },TEST_COMP_PERIOD,false,true);
    
    ComHandler::MapTestIdCallback(this->_testIdCallback);
    this->testId.reserve(TEST_ID_LENGTH);
    this->ClearTestId();
    RegisterChild(ackStartTimer);
    RegisterChild(ackCompleteTimer);
}

void TestController::SetConfig(const BurnTimerConfig timerConfig){
    this->burn_timer.SetConfig(timerConfig);
    this->ClearTestId();
}

void TestController::ClearTestId(){
    this->testIdSet=false;
    for(int i=0;i<TEST_ID_LENGTH;i++){
        this->testId[i]='x';
    }
}

void TestController::AcknowledgeTestStart(){
    this->ackStartTimer.cancel();
}

void TestController::AcknowledgeTestComplete(){
    this->ackCompleteTimer.cancel();
}

void TestController::GetProbeRunTimeOkay(bool *probeRtOkay){
    this->burn_timer.GetProbeTimeOkay(probeRtOkay);
}

const char* TestController::GetTestId(){
    return this->testId.c_str();
}

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
    this->testIdSet=false;
    for(int i=0;i<TEST_ID_LENGTH;i++){
        this->testId[i]='x';
    }
    this->ackStartTimer.cancel();
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
    if(!this->testIdSet){
        ComHandler::SendErrorMessage(SystemError::TEST_ID_NOT_SET);
        return false;
    } 
    this->currentSet=true;
    this->stressCurrent=current;
    this->burn_timer.Start(this->stressCurrent);
    this->ackStartTimer.onInterval([&](){
        ComHandler::SendStartResponse(true,F("Test Started"));
    },TEST_START_PERIOD,true,true);
    return true;
}

bool TestController::StartTest(SaveState state){
    this->timerDataSate=state.currentTimes;
    this->saveState=state;
    this->savedStateLoaded=true;
    this->testId=state.testId;
    this->currentSet=true;
    this->stressCurrent=state.setCurrent;
    this->testIdSet=true;
    this->ackStartTimer.onInterval([&](){
        ComHandler::MsgPacketSerializer(this->saveState,PacketType::TEST_LOAD_START);
    },2000,true,true);
    return this->burn_timer.Start(this->timerDataSate);
}

void TestController::SendRunningTest(){
    if(this->IsRunning()){
        ComHandler::MsgPacketSerializer(this->saveState,PacketType::SEND_RUNNING_TEST);
    }
}

void TestController::CompleteTest(){
        ComHandler::SendSystemMessage(SystemMessage::TEST_STATE_COMPLETED,MessageType::NOTIFY);
        this->Reset();
        this->ackStartTimer.cancel();
        this->ackCompleteTimer.start();
        this->_finishedCallback();
}

void TestController::SetTestId(const char* id){
    if(this->IsRunning()){
        ComHandler::SendErrorMessage(SystemError::TEST_RUNNING_ERR);
        return;
    }
    this->testId=id;
    this->testIdSet=true;
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
    return true;
}

