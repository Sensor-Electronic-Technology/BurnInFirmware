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

class State {
  friend class SimpleFSM;

 public:
  State();
  State(String name, CallbackFunction on_enter, CallbackFunction on_state = [](){_NOP();}, CallbackFunction on_exit = [](){_NOP();}, bool is_final = false);

  void setup(String name, CallbackFunction on_enter, CallbackFunction on_state = [](){_NOP();}, CallbackFunction on_exit = [](){_NOP();}, bool is_final = false);
  void setName(String name);
  void setOnEnterHandler(CallbackFunction f);
  void setOnStateHandler(CallbackFunction f);
  void setOnExitHandler(CallbackFunction f);
  void setAsFinal(bool final = true);

  int getID() const;
  bool isFinal() const;
  String getName() const;

 protected:
  int id;
  static int _next_id;

  String name = "";
  CallbackFunction on_enter = [](){_NOP();};
  CallbackFunction on_state = [](){_NOP();};
  CallbackFunction on_exit = [](){_NOP();};
  bool is_final = false;
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
