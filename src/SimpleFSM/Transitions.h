/////////////////////////////////////////////////////////////////

#pragma once
#ifndef TRANSITIONS_H
#define TRANSITIONS_H

/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "Handlers.h"
#include "State.h"


/////////////////////////////////////////////////////////////////
// abstract parent class for Transition and TimedTransition

class AbstractTransition {
  friend class SimpleFSM;

 public:
  AbstractTransition();
  // to make this class an interface
  virtual ~AbstractTransition(){};
  virtual int getID() const = 0;
  int getTransitionId() const;

  void setTransitionId(int transitionId);
  void setOnRunHandler(CallbackFunction f);
  void setGuardCondition(GuardCondition f);

 protected:
  static int _next_id;
  int id = 0;
  int transition_id =-1;
  State* from = NULL;
  State* to = NULL;
  CallbackFunction on_run_cb = [](){};
  GuardCondition guard_cb = [](){ return true; };
};

/////////////////////////////////////////////////////////////////

class Transition : public AbstractTransition {
  friend class SimpleFSM;

 public:
  Transition();
  Transition(State* from, State* to, int event_id, int transitionId = -1,CallbackFunction on_run = [](){}, GuardCondition guard = [](){ return true; });

  void setup(State* from, State* to, int event_id,int transitionId = -1, CallbackFunction on_run = [](){}, GuardCondition guard = [](){ return true; });

  int getID() const;
  int getEventID() const;

 protected:
  int event_id;
};

/////////////////////////////////////////////////////////////////

class TimedTransition : public AbstractTransition {
  friend class SimpleFSM;

 public:
  TimedTransition();
  TimedTransition(State* from, State* to, int interval, int transitionId = -1,CallbackFunction on_run = [](){}, GuardCondition guard = [](){ return true; });

  void setup(State* from, State* to, int interval, int transitionId = -1,CallbackFunction on_run = [](){}, GuardCondition guard = [](){ return true; });

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
