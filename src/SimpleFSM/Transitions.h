/////////////////////////////////////////////////////////////////

#pragma once
#ifndef TRANSITIONS_H
#define TRANSITIONS_H

/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "Handlers.h"
#include "State.h"

/////////////////////////////////////////////////////////////////

// typedef void (*CallbackFunction)();
// typedef bool (*GuardCondition)();

/////////////////////////////////////////////////////////////////
// abstract parent class for Transition and TimedTransition
template<typename SID,typename TID>
class AbstractTransition {
  template<typename,typename>
  friend class SimpleFSM;

 public:
  AbstractTransition();
  // to make this class an interface
  virtual ~AbstractTransition(){};
  virtual int getID() const = 0;
  TID getTransitionId() const;

  void setTransitionId(TID t_id);
  void setOnRunHandler(CallbackFunction f);
  void setGuardCondition(GuardCondition f);

 protected:
  static int _next_id;
  int id = 0;
  bool                  hasTransitionId=false;
  TID                   transitionId;
  State<SID>*           from = NULL;
  State<SID>*           to = NULL;
  CallbackFunction      on_run_cb = [](){_NOP();};
  GuardCondition        guard_cb = [](){return true;};
};

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
class Transition : public AbstractTransition<SID,TID> {
  // template<typenaname,typename>
  // friend class SimpleFSM;

 public:
  Transition();
  Transition(State<SID>* from, State<SID>* to, int event_id,TID t_id, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard =[](){return true;});
  Transition(State<SID>* from, State<SID>* to, int event_id, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard =[](){return true;});

  void setup(State<SID>* from, State<SID>* to, int event_id, TID t_id,CallbackFunction on_run = [](){_NOP();}, GuardCondition guard = [](){return true;});
  void setup(State<SID>* from, State<SID>* to, int event_id, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard = [](){return true;});

  int getID() const;
  int getEventID() const;

 protected:
  int event_id;
};

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
class TimedTransition : public AbstractTransition<SID,TID> {
  // template<typenaname,typename>
  // friend class SimpleFSM;

 public:
  TimedTransition();
  TimedTransition(State<SID>* from, State<SID>* to, int interval,TID t_id, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard = [](){return true;});

  void setup(State<SID>* from, State<SID>* to, int interval,TID t_id, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard = [](){return true;});

  TimedTransition(State<SID>* from, State<SID>* to, int interval, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard = [](){return true;});

  void setup(State<SID>* from, State<SID>* to, int interval, CallbackFunction on_run = [](){_NOP();}, GuardCondition guard = [](){return true;});

  int getID() const;
  int getInterval() const;

  void reset();

 protected:
    unsigned long start;
    unsigned long interval;
};
/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
