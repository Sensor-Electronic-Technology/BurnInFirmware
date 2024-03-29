/////////////////////////////////////////////////////////////////
#include "State.h"
/////////////////////////////////////////////////////////////////
template<typename SID>
int State<SID>::_next_id = 0;

/////////////////////////////////////////////////////////////////
template<typename SID>
State<SID>::State() {
}

/////////////////////////////////////////////////////////////////
template<typename SID>
State<SID>::State(SID stateId, CallbackFunction on_enter, CallbackFunction on_state, CallbackFunction on_exit, bool is_final /* = false */) {
  setup(stateId, on_enter, on_state, on_exit, is_final);
}

/////////////////////////////////////////////////////////////////
template<typename SID>
void State<SID>::setup(SID stateId, CallbackFunction on_enter, CallbackFunction on_state, CallbackFunction on_exit, bool is_final /* = false */) {
  this->state_id = stateId;
  this->on_enter = on_enter;
  this->on_state = on_state;
  this->on_exit = on_exit;
  this->id = _next_id++;
  this->is_final = is_final;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
SID State<SID>::getStateId() const {
  return state_id;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
bool State<SID>::isFinal() const {
  return is_final;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
void State<SID>::setStateId(SID name) {
  this->state_id = name;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
void State<SID>::setOnEnterHandler(CallbackFunction f) {
  this->on_enter = f;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
void State<SID>::setOnStateHandler(CallbackFunction f) {
  this->on_state = f;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
void State<SID>::setOnExitHandler(CallbackFunction f) {
  this->on_exit = f;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
int State<SID>::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////
template<typename SID>
void State<SID>::setAsFinal(bool final /* = true */) {
  is_final = final;
}

/////////////////////////////////////////////////////////////////
