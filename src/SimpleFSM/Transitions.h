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

class AbstractTransition {
  friend class SimpleFSM;

 public:
  AbstractTransition();
  // to make this class an interface
  virtual ~AbstractTransition(){};
  virtual int getID() const = 0;
  String getName() const;

  void setName(String name);
  void setOnRunHandler(CallbackFunction f);
  void setGuardCondition(GuardCondition f);

 protected:
  static int _next_id;
  int id = 0;
  String name = "";
  State* from = NULL;
  State* to = NULL;
  CallbackFunction on_run_cb = [](){return true;};
  GuardCondition guard_cb = [](){return true;};
};

/////////////////////////////////////////////////////////////////

class Transition : public AbstractTransition {
  friend class SimpleFSM;

 public:
  Transition();
  Transition(State* from, State* to, int event_id, CallbackFunction on_run = [](){_NOP();}, String name = "", GuardCondition guard =[](){return true;});

  void setup(State* from, State* to, int event_id, CallbackFunction on_run = [](){_NOP();}, String name = "", GuardCondition guard = [](){return true;});

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
  TimedTransition(State* from, State* to, int interval, CallbackFunction on_run = [](){_NOP();}, String name = "", GuardCondition guard = [](){return true;});

  void setup(State* from, State* to, int interval, CallbackFunction on_run = [](){_NOP();}, String name = "", GuardCondition guard = [](){return true;});

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
