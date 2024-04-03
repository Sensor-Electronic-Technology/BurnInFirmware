#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../StateMachine/StateMachine.hpp"
#include "../Files/FileManager.hpp"
#include "../Logging/StationLogger.hpp"
#include "heater_constants.h"

#define HEATER_DEBUG 				1
#define HEATER_STATE_COUNT 			3
#define HEATER_TRANSITION_COUNT 	4
#define TUNE_STATE_COUNT 			3
#define TUNE_TRANSITION_COUNT 		5
#define MODE_TRANSITION_COUNT 2

using namespace components;

class HeaterController:public Component{
public:
    HeaterController(const HeaterControllerConfig& config);
    HeaterController();
    void Setup(const HeaterControllerConfig& config);
    void Initialize();
    void ToggleHeaters();
    void TurnOn();
    void TurnOff();
    void ReadTemperatures();
    bool TempOkay();
    void ChangeSetPoint(uint8_t setPoint);
    

    void GetResults(HeaterResult* fill);
    void Print();
    inline bool IsTuning(){
        return this->isTuning;
    }

    void RunAutoTune();
    void RunHeating();

    //Tuning Commands
    void StartTuning();
    void StopTuning();
    //Tuning Run

    //Run Actions
    void TuningRun();
    void TuningIdle();
    void TuningComplete();

    void Run();
    void WarmupRun(){
        if(this->TempOkay()){
            this->heatersStateMachine.triggerEvent(HeaterTrigger::TEMP_REACHED);
        }
    }
    void HeatingIdle();

    

    void HandleResponse(Response response);
    void TuningComplete(HeaterTuneResult result);


    //Heating
    void OnIdleToTuning(){
        this->StartTuning();
    }

    void OnTuningToIdle(){
        this->StopTuning();
    }

    void OnTuningToComplete(){
        
    }

    void OnCompleteToIdle(){

    }
    

    //Mode

    bool SwitchToAutoTune(){
        return this->modeStateMachine.triggerEvent(ModeTrigger::ATUNE_START);
    }

    void OnHeatingToAutoTune(){
        this->TurnOff();
        this->tuningStateMachine.Reset();
    }

    void OnAutoTuneToHeating(){
        this->StopTuning();
        this->heatersStateMachine.Reset();
    }

    bool SwitchToHeating(){
        return this->modeStateMachine.triggerEvent(ModeTrigger::HEATING_START);
    }

private:
    Heater heaters[HEATER_COUNT];
    HeaterResult results[HEATER_COUNT];
    AutoTuneResults tuningResults;
    Timer  readTimer,printTimer;
    HeatState heaterState=HeatState::Off;
    StateMachine<HEATER_TRANSITION_COUNT> heatersStateMachine;
    StateMachine<TUNE_TRANSITION_COUNT> tuningStateMachine;
    StateMachine<MODE_TRANSITION_COUNT> modeStateMachine;

    HeaterControllerConfig configuration;
    TuningCompleteCallback tuningCompleteCbk=[](HeaterTuneResult){};
    ResponseCallback responseCbk=[](Response){};
    unsigned long readInterval;
    bool isTuning=false;
    bool tuningCompleted=false;
    virtual void privateLoop() override;

    State heaterStates[HEATER_STATE_COUNT]={
		State(HeaterState::HEATER_OFF,[&](){this->HeatingIdle();}),
		State(HeaterState::HEATER_WARMUP,[&](){this->WarmupRun();}),
		State(HeaterState::HEATER_ON,[&](){this->Run();})
	};

	Transition heaterTransitions[HEATER_TRANSITION_COUNT]={
		Transition(&heaterStates[HEATER_OFF],&heaterStates[HEATER_WARMUP],
                HeaterTransition::HEATER_OFF_TO_WARMUP,
                HeaterTrigger::START_HEATERS,
                [&](){this->TurnOn();},
                [](){return true;}),
		Transition(&heaterStates[HEATER_WARMUP],&heaterStates[HEATER_ON],
                HeaterTransition::HEATER_WARMUP_TO_ON,
                HeaterTrigger::TEMP_REACHED,
                [](){},
                [](){return true;}),
		Transition(&heaterStates[HEATER_ON],&heaterStates[HEATER_OFF],
                HeaterTransition::HEATER_ON_TO_OFF,
                HeaterTrigger::STOP_HEATERS,
                [](){},
                [](){return true;}),
		Transition(&heaterStates[HEATER_WARMUP],&heaterStates[HEATER_OFF],
                HeaterTransition::HEATER_WARMUP_TO_OFF,
                HeaterTrigger::STOP_HEATERS,
                [](){},
                [](){return true;}),
	};

	State tuneStates[TUNE_STATE_COUNT]={
		State(TuneState::TUNE_IDLE,[&](){}),
		State(TuneState::TUNE_RUNNING,[&](){this->TuningRun();}),
		State(TuneState::TUNE_COMPLETE,[&](){})
	};

	Transition tuneTransitions[TUNE_TRANSITION_COUNT]={
		Transition(&tuneStates[TuneState::TUNE_IDLE],&tuneStates[TuneState::TUNE_RUNNING],
                TuneTransition::TUNE_IDLE_TO_RUNNING,
                TuneTrigger::TUNE_START,
                [](){},
                [](){return true;}),

		Transition(&tuneStates[TuneState::TUNE_RUNNING],&tuneStates[TuneState::TUNE_COMPLETE],
                TuneTransition::TUNE_RUNNING_TO_IDLE,
                TuneTrigger::TUNE_FINISHED,
                [](){},
                [](){return true;}),

		Transition(&tuneStates[TuneState::TUNE_COMPLETE],&tuneStates[TuneState::TUNE_IDLE],
                TuneTransition::TUNE_COMPLETE_TO_IDLE,
                TuneTrigger::TUNE_SAVE,
                [](){},
                [](){return true;}),

		Transition(&tuneStates[TuneState::TUNE_COMPLETE],&tuneStates[TuneState::TUNE_IDLE],
                TuneTransition::TUNE_COMPLETE_TO_IDLE,
                TuneTrigger::TUNE_CANCEL,
                [](){},
                [](){return true;}),

		Transition(&tuneStates[TuneState::TUNE_RUNNING],&tuneStates[TuneState::TUNE_IDLE],
                TuneTransition::TUNE_RUNNING_TO_IDLE,
                TuneTrigger::TUNE_STOP,
                [](){},
                [](){return true;})
	};

    State modes[2]={
        State(HeaterMode::HEATING,[&](){this->RunHeating();}),
        State(HeaterMode::ATUNE,[&](){this->RunAutoTune();}),
    };

    Transition modeTransitions[2]={
        Transition(&modes[HeaterMode::HEATING],&modes[HeaterMode::ATUNE],
                ModeTransition::MODE_HEATING_TO_ATUNE,
                ModeTrigger::ATUNE_START,
                [&](){this->OnHeatingToAutoTune();},
                [](){return true;}),
        Transition(&modes[HeaterMode::ATUNE],&modes[HeaterMode::HEATING],
                ModeTransition::MODE_ATUNE_TO_HEATING,
                ModeTrigger::HEATING_START,
                [&](){this->OnAutoTuneToHeating();},
                [](){return true;}),
    };

};