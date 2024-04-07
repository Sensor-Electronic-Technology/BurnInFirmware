#pragma once
#include <ArduinoComponents.h>
#include <Array.h>
#include "HeaterConfiguration.hpp"
#include "Heater.hpp"
#include "../StateMachine/StateMachine.hpp"
#include "../Files/FileManager.hpp"
#include "heater_constants.h"

#define HEATER_DEBUG 				1
#define HEATER_STATE_COUNT 			3
#define HEATER_TRANSITION_COUNT 	4
#define TUNE_STATE_COUNT 			3
#define TUNE_TRANSITION_COUNT 		5
#define MODE_TRANSITION_COUNT       2

using namespace components;

class HeaterController:public Component{
public:
    HeaterController(const HeaterControllerConfig& config);
    HeaterController();
    void Setup(const HeaterControllerConfig& config);
    void Initialize();
    void ReadTemperatures();
    bool TempOkay();
    void ChangeSetPoint(uint8_t setPoint);
    void GetResults(HeaterResult* fill);
    void Print();

    bool CanTuningTransition();
    bool CanHeatingTransition();

    inline bool IsTuning(){
        return this->isTuning;
    }
#pragma region Mode

    //Modes
    void RunAutoTune();
    void RunHeating();

    //Mode Transitions
    bool SwitchToAutoTune();
    void OnHeatingToAutoTune();
    void OnAutoTuneToHeating();
    bool SwitchToHeating();

#pragma region Heating
    //Heating States
    void RunOn();
    void WarmupRun();
    void TurnOn();
    void TurnOff();

    void OnTurnOn(); 
    void OnTurnOff();

    void ToggleHeaters();
#pragma endregion


#pragma region Tuning
    //Tuning States
    void TuningRun();
    //Tuning Commands
    bool StartTuning();
    bool StopTuning();
    bool SaveTuning();
    bool DiscardTuning();

    //Tuning Callbacks
    void HeaterRunCompleteHandler(HeaterTuneResult result);
    //Tune Transitions
    void OnIdleToTuning();
    void OnStopTuning();

    void OnCompleteToIdleSave();
    void OnCompleteToIdleDiscard();
#pragma endregion


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
    unsigned long readInterval;
    bool isTuning=false;
    bool tuningCompleted=false;
    virtual void privateLoop() override;

    State modes[MODE_TRANSITION_COUNT]={
        State(HeaterMode::HEATING,[&](){this->RunHeating();}),
        State(HeaterMode::ATUNE,[&](){this->RunAutoTune();}),
    };

    Transition modeTransitions[MODE_TRANSITION_COUNT]={
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

    State heaterStates[HEATER_STATE_COUNT]={
        State(HeaterState::HEATER_OFF,[&](){_NOP();}),
        State(HeaterState::HEATER_WARMUP,[&](){this->WarmupRun();}),
        State(HeaterState::HEATER_ON,[&](){this->RunOn();})
    };

    Transition heaterTransitions[HEATER_TRANSITION_COUNT]={
        Transition(&heaterStates[HeaterState::HEATER_OFF],&heaterStates[HeaterState::HEATER_WARMUP],
            HeaterTransition::HEATER_OFF_TO_WARMUP,
            HeaterTrigger::START_HEATERS,
            [&](){this->OnTurnOn();},
            [&](){return this->CanHeatingTransition();}),
        Transition(&heaterStates[HeaterState::HEATER_WARMUP],&heaterStates[HeaterState::HEATER_ON],
            HeaterTransition::HEATER_WARMUP_TO_ON,
            HeaterTrigger::TEMP_REACHED,
            [](){_NOP();},
            [&](){return this->CanHeatingTransition();}),
        Transition(&heaterStates[HeaterState::HEATER_ON],&heaterStates[HeaterState::HEATER_OFF],
            HeaterTransition::HEATER_ON_TO_OFF,
            HeaterTrigger::STOP_HEATERS,
            [&](){this->OnTurnOff();},
            [&](){return this->CanHeatingTransition();}),
         Transition(&heaterStates[HEATER_WARMUP],&heaterStates[HEATER_OFF],
            HeaterTransition::HEATER_WARMUP_TO_OFF,
            HeaterTrigger::STOP_HEATERS,
            [&](){this->OnTurnOff();},
            [&](){return this->CanHeatingTransition();}) 
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
                [&](){this->OnIdleToTuning();},
                [&](){return this->CanTuningTransition();}),
        Transition(&tuneStates[TuneState::TUNE_RUNNING],&tuneStates[TuneState::TUNE_IDLE],
                TuneTransition::TUNE_RUNNING_TO_IDLE,
                TuneTrigger::TUNE_STOP,
                [&](){this->OnStopTuning();},
                [&](){return this->CanTuningTransition();}),
        Transition(&tuneStates[TuneState::TUNE_RUNNING],&tuneStates[TuneState::TUNE_COMPLETE],
                TuneTransition::TUNE_RUNNING_TO_COMPLETE,
                TuneTrigger::TUNE_FINISHED,
                [&](){return true;}),
        Transition(&tuneStates[TuneState::TUNE_COMPLETE],&tuneStates[TuneState::TUNE_IDLE],
                TuneTransition::TUNE_COMPLETE_TO_IDLE,
                TuneTrigger::TUNE_SAVED,
                [&](){this->OnCompleteToIdleSave();},
                [&](){return this->CanTuningTransition();}),
        Transition(&tuneStates[TuneState::TUNE_COMPLETE],&tuneStates[TuneState::TUNE_IDLE],
                TuneTransition::TUNE_COMPLETE_TO_IDLE,
                TuneTrigger::TUNE_DISCARD,
                [&](){this->OnCompleteToIdleDiscard();},
                [&](){return this->CanTuningTransition();}),
        };

};