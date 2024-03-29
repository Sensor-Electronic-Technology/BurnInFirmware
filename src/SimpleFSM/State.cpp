/////////////////////////////////////////////////////////////////
#include "State.h"
/////////////////////////////////////////////////////////////////
template <typename T>
int State<T>::_next_id = 0;

/////////////////////////////////////////////////////////////////
template <typename T>
State<T>::State() {
}

/////////////////////////////////////////////////////////////////
template <typename T>
State<T>::State(T stateId, CallbackFunction on_enter, CallbackFunction on_state, CallbackFunction on_exit, bool is_final /* = false */) {
  setup(name, on_enter, on_state, on_exit, is_final);
}

/////////////////////////////////////////////////////////////////
template <typename T>
void State<T>::setup(T stateId, CallbackFunction on_enter, CallbackFunction on_state, CallbackFunction on_exit, bool is_final /* = false */) {
  this->state_id = name;
  this->on_enter = on_enter;
  this->on_state = on_state;
  this->on_exit = on_exit;
  this->id = _next_id++;
  this->is_final = is_final;
}

/////////////////////////////////////////////////////////////////
template <typename T>
T State<T>::getStateId() const {
  return state_id;
}

/////////////////////////////////////////////////////////////////
template <typename T>
bool State<T>::isFinal() const {
  return is_final;
}

/////////////////////////////////////////////////////////////////
template <typename T>
void State<T>::setStateId(T name) {
  this->state_id = name;
}

/////////////////////////////////////////////////////////////////
template <typename T>
void State<T>::setOnEnterHandler(CallbackFunction f) {
  this->on_enter = f;
}

/////////////////////////////////////////////////////////////////
template <typename T>
void State<T>::setOnStateHandler(CallbackFunction f) {
  this->on_state = f;
}

/////////////////////////////////////////////////////////////////
template <typename T>
void State<T>::setOnExitHandler(CallbackFunction f) {
  this->on_exit = f;
}

/////////////////////////////////////////////////////////////////
template <typename T>
int State<T>::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////
template <typename T>
void State<T>::setAsFinal(bool final /* = true */) {
  is_final = final;
}

/////////////////////////////////////////////////////////////////
