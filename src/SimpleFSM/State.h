/////////////////////////////////////////////////////////////////

#pragma once
#ifndef STATE_H
#define STATE_H

/////////////////////////////////////////////////////////////////

#include "Arduino.h"
#include "Handlers.h"

/////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////

class State {
  friend class SimpleFSM;

 public:
  State();
  State(int stateId, CallbackFunction on_enter=[](){}, CallbackFunction on_state = [](){}, CallbackFunction on_exit = [](){}, bool is_final = false);

  void setup(int stateId, CallbackFunction on_enter=[](){}, CallbackFunction on_state = [](){}, CallbackFunction on_exit = [](){}, bool is_final = false);
  void setStateId(int stateId);
  void setOnEnterHandler(CallbackFunction f);
  void setOnStateHandler(CallbackFunction f);
  void setOnExitHandler(CallbackFunction f);
  void setAsFinal(bool final = true);

  int getID() const;
  bool isFinal() const;
  int getStateId() const;

 protected:
  int id;
  static int _next_id;

  int state_id=0;
  CallbackFunction on_enter = [](){};
  CallbackFunction on_state = [](){};
  CallbackFunction on_exit = [](){};
  bool is_final = false;
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
