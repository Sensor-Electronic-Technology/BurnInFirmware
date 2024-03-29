/////////////////////////////////////////////////////////////////
#include "Transitions.h"
/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
int AbstractTransition<SID,TID>::_next_id = 0;

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
AbstractTransition<SID,TID>::AbstractTransition() {
  id = _next_id;
  _next_id++;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void AbstractTransition<SID,TID>::setGuardCondition(GuardCondition f) {
  guard_cb = f;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
String AbstractTransition<SID,TID>::getName() const {
  return name;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void AbstractTransition<SID,TID>::setName(String name) {
  this->name = name;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void AbstractTransition<SID,TID>::setOnRunHandler(CallbackFunction f) {
  on_run_cb = f;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// Transition
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
int Transition<SID,TID>::getEventID() const {
  return event_id;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
int Transition<SID,TID>::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
Transition<SID,TID>::Transition() : event_id(0) {}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
Transition<SID,TID>::Transition(State<SID>* from, State<SID>* to, int event_id, CallbackFunction on_run /* = NULL */,TID t_id /* = "" */, GuardCondition guard /* = NULL */) {
  setup(from, to, event_id, on_run, name, guard);
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void Transition<SID,TID>::setup(State<SID>* from, State<SID>* to, int event_id, CallbackFunction on_run /* = NULL */, TID t_id /* = "" */, GuardCondition guard /* = NULL */) {
  this->from = from;
  this->to = to;
  this->event_id = event_id;
  this->on_run_cb = on_run;
  this->name = name;
  this->guard_cb = guard;
}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// TimedTransition
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
TimedTransition<SID,TID>::TimedTransition() : start(0), interval(0) {}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
TimedTransition<SID,TID>::TimedTransition(State<SID>* from, State<SID>* to, int interval, CallbackFunction on_run /* = NULL */, TID t_id /* = "" */, GuardCondition guard /* = NULL */) : TimedTransition() {
  setup(from, to, interval, on_run, name, guard);
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void TimedTransition<SID,TID>::setup(State<SID>* from, State<SID>* to, int interval, CallbackFunction on_run /* = NULL */, TID t_id /* = "" */, GuardCondition guard /* = NULL */) {
  this->from = from;
  this->to = to;
  this->interval = interval;
  this->on_run_cb = on_run;
  this->name = name;
  this->guard_cb = guard;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
int TimedTransition<SID,TID>::getInterval() const {
  return interval;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
int TimedTransition<SID,TID>::getID() const {
  return id;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void TimedTransition<SID,TID>::reset() {
  start = 0;
}

/////////////////////////////////////////////////////////////////