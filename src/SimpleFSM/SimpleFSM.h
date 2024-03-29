/////////////////////////////////////////////////////////////////

#pragma once
#ifndef SIMPLE_FSM_H
#define SIMPLE_FSM_H

/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "State.h"
#include "Handlers.h"
#include "Transitions.h"

/////////////////////////////////////////////////////////////////

// typedef void (*CallbackFunction)();
// typedef bool (*GuardCondition)();


/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
class SimpleFSM {
 public:
  SimpleFSM();
  SimpleFSM(State<SID>* initial_state);
  ~SimpleFSM();

  void add(Transition<SID,TID> t[], int size);
  void add(TimedTransition<SID,TID> t[], int size);

  void setInitialState(State<SID>* state);
  void setFinishedHandler(CallbackFunction f);
  void setTransitionHandler(CallbackFunction f);

  bool trigger(int event_id);
  void run(int interval = 1000, CallbackFunction tick_cb = NULL);
  void reset();

  int getTransitionCount() const;
  int getTimedTransitionCount() const;
  
  bool isFinished() const;
  State<SID>* getState() const;
  bool isInState(State<SID>* state) const;
  State<SID>* getPreviousState() const;
  unsigned long lastTransitioned() const;
  // String getDotDefinition();

 protected:
  int num_timed = 0;
  int num_standard = 0;
  Transition<SID,TID>* transitions = NULL;
  TimedTransition<SID,TID>* timed = NULL;

  bool is_initialized = false;
  bool is_finished = false;
  unsigned long last_run = 0;
  unsigned long last_transition = 0;

  State<SID>* inital_state = NULL;
  State<SID>* current_state = NULL;
  State<SID>* prev_state = NULL;
  CallbackFunction on_transition_cb = [](){_NOP();};
  CallbackFunction finished_cb = [](){_NOP();};

  //String dot_definition = "";

  bool _isDuplicate(const TimedTransition<SID,TID>& transition, const TimedTransition<SID,TID>* transitionArray, int arraySize) const;
  bool _isDuplicate(const Transition<SID,TID>& transition, const Transition<SID,TID>* transitionArray, int arraySize) const;

  bool _isTimeForRun(unsigned long now, int interval);
  void _handleTimedEvents(unsigned long now);
  
  bool _initFSM();
  bool _transitionTo(AbstractTransition<SID,TID>* transition);
  bool _changeToState(State<SID>* s, unsigned long now);

  // void _addDotTransition(Transition& t);
  // void _addDotTransition(TimedTransition& t);
  // String _dot_transition(String from, String to, String label, String param);
  // String _dot_inital_state();
  // String _dot_header();
  // String _dot_active_node();
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
