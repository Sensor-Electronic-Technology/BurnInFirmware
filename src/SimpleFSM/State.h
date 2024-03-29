/////////////////////////////////////////////////////////////////

#pragma once
#ifndef STATE_H
#define STATE_H

/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "Handlers.h"

/////////////////////////////////////////////////////////////////

// typedef void (*CallbackFunction)();
// typedef bool (*GuardCondition)();

/////////////////////////////////////////////////////////////////
template <typename T>
class State {
  friend class SimpleFSM;

 public:
  State();
  State(T stateId, CallbackFunction on_enter, CallbackFunction on_state = [](){_NOP();}, CallbackFunction on_exit = [](){_NOP();}, bool is_final = false);

  void setup(T stateId, CallbackFunction on_enter, CallbackFunction on_state = [](){_NOP();}, CallbackFunction on_exit = [](){_NOP();}, bool is_final = false);
  void setStateId(T stateId);
  void setOnEnterHandler(CallbackFunction f);
  void setOnStateHandler(CallbackFunction f);
  void setOnExitHandler(CallbackFunction f);
  void setAsFinal(bool final = true);

  int getID() const;
  bool isFinal() const;
  T getStateId() const;

 protected:
  int id;
  static int _next_id;

  T state_id;
  CallbackFunction on_enter = [](){_NOP();};
  CallbackFunction on_state = [](){_NOP();};
  CallbackFunction on_exit = [](){_NOP();};
  bool is_final = false;
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
