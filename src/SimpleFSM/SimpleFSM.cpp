/////////////////////////////////////////////////////////////////
#include "SimpleFSM.h"

#include "State.h"
#include "Transitions.h"
///////////////////
template<typename SID,typename TID>
SimpleFSM<SID,TID>::SimpleFSM() {
}

/////////////////////////////////////////////////////////////////
/*
 * Constructor.
 */
template<typename SID,typename TID>
SimpleFSM<SID,TID>::SimpleFSM(State<SID>* initial_state) {
  SimpleFSM();
  setInitialState(initial_state);
}

/////////////////////////////////////////////////////////////////
/*
 * Destructor.
 */
template<typename SID,typename TID>
SimpleFSM<SID,TID>::~SimpleFSM() {
  transitions = NULL;
  timed = NULL;
}

/////////////////////////////////////////////////////////////////
/*
 * Get the number of transitions.
 */
template<typename SID,typename TID>
int SimpleFSM<SID,TID>::getTransitionCount() const {
  return num_standard;
}

/////////////////////////////////////////////////////////////////
/*
 * Get the number of timed transitions.
 */
template<typename SID,typename TID>
int SimpleFSM<SID,TID>::getTimedTransitionCount() const {
  return num_timed;
}

/////////////////////////////////////////////////////////////////
/*
 * Reset the FSM to its initial state.
 */
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::reset() {
  is_initialized = false;
  is_finished = false;
  last_run = 0;
  last_transition = 0;
  setInitialState(inital_state);
  current_state = NULL;
  prev_state = NULL;

  for (int i = 0; i < num_timed; i++) {
    timed[i].reset();
  }
}

/////////////////////////////////////////////////////////////////
/*
 * Set the initial state.
 */
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::setInitialState(State<SID>* state) {
  inital_state = state;
}

/////////////////////////////////////////////////////////////////
/*
 * Trigger an event.
 */
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::trigger(int event_id) {
  if (!is_initialized) _initFSM();
  // Find the transition with the current state and given event
  for (int i = 0; i < num_standard; i++) {
    if (transitions[i].from == current_state && transitions[i].event_id == event_id) {
      return _transitionTo(&(transitions[i]));
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////
/*
 * Get the previous state.
 */ 
template<typename SID,typename TID>
State<SID>* SimpleFSM<SID,TID>::getPreviousState() const {
  return prev_state;
}

/////////////////////////////////////////////////////////////////
/*
 * Get the current state.
 */
template<typename SID,typename TID>
State<SID>* SimpleFSM<SID,TID>::getState() const {
  return current_state;
}

/////////////////////////////////////////////////////////////////
/*
 * Check if the FSM is in a given state.
 */
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::isInState(State<SID>* t) const {
  return t == current_state;
}

/////////////////////////////////////////////////////////////////
/*
 * Sets the transition handler.
 */
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::setTransitionHandler(CallbackFunction f) {
  on_transition_cb = f;
}

/////////////////////////////////////////////////////////////////
/* 
 * Add transitions to the FSM.
 * 
 * @param t[] An array of transitions.
 * @param size The size of the array.  
 */
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::add(Transition<SID,TID> newTransitions[], int size) {
  // Count the number of unique transitions
  int uniqueCount = 0;
  for (int i = 0; i < size; ++i) {
    if (!_isDuplicate(newTransitions[i], transitions, num_standard) && 
        !_isDuplicate(newTransitions[i], newTransitions, i)) {
      uniqueCount++;
    }
  }
  // Allocate or expand storage for transitions with exact size
  Transition<SID,TID>* temp = new Transition<SID,TID>[num_standard + uniqueCount];
  if (transitions != NULL) {
    memcpy(temp, transitions, num_standard * sizeof(Transition<SID,TID>));
    delete[] transitions;
  }
  transitions = temp;
  // Check if memory allocation was successful
  if (transitions == NULL) {
    Serial.print("Out of storage");
    abort();
  }
  // Add new transitions, avoiding duplicates
  for (int i = 0; i < size; ++i) {
    if (!_isDuplicate(newTransitions[i], transitions, num_standard) && 
        !_isDuplicate(newTransitions[i], newTransitions, i)) {
      transitions[num_standard] = newTransitions[i];
      //_addDotTransition(transitions[num_standard]);
      num_standard++;
    }
  }
}

/////////////////////////////////////////////////////////////////
/*
  * Add timed transitions to the FSM.
  * 
  * @param t[] An array of timed transitions.
  * @param size The size of the array.  
  */
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::add(TimedTransition<SID,TID> newTransitions[], int size) {
  // Count the number of unique transitions
  int uniqueCount = 0;
  for (int i = 0; i < size; ++i) {
    if (!_isDuplicate(newTransitions[i], timed, num_timed) && 
        !_isDuplicate(newTransitions[i], newTransitions, i)) {
      uniqueCount++;
    }
  }
  // Allocate memory or expand existing storage with exact size
  TimedTransition<SID,TID>* temp = new TimedTransition<SID,TID>[num_timed + uniqueCount];
  if (timed != NULL) {
    memcpy(temp, timed, num_timed * sizeof(TimedTransition<SID,TID>));
    delete[] timed;
  }
  timed = temp;
  // Check memory allocation
  if (timed == NULL) {
    Serial.print("Out of storage");
    abort();
  }
  // Add new transitions while avoiding duplicates
  for (int i = 0; i < size; ++i) {
    if (!_isDuplicate(newTransitions[i], timed, num_timed) && 
        !_isDuplicate(newTransitions[i], newTransitions, i)) {
      timed[num_timed] = newTransitions[i];
      //_addDotTransition(timed[num_timed]);
      num_timed++;
    }
  }
}

/////////////////////////////////////////////////////////////////
/*
 * Check if a timed transition is a duplicate.
 */
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::_isDuplicate(const TimedTransition<SID,TID>& transition, const TimedTransition<SID,TID>* transitionArray, int arraySize) const {
  for (int i = 0; i < arraySize; ++i) {
    if (transitionArray[i].from == transition.from &&
        transitionArray[i].to == transition.to &&
        transitionArray[i].interval == transition.interval) {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////
/*
 * Check if a transition is a duplicate.
 */
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::_isDuplicate(const Transition<SID,TID>& transition, const Transition<SID,TID>* transitionArray, int arraySize) const {
  for (int i = 0; i < arraySize; ++i) {
    if (transitionArray[i].from == transition.from &&
        transitionArray[i].to == transition.to &&
        transitionArray[i].event_id == transition.event_id) {
      return true;
    }
  }
  return false;
}

/////////////////////////////////////////////////////////////////
/*
 * Set the finished handler.
 */
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::setFinishedHandler(CallbackFunction f) {
  finished_cb = f;
}

/////////////////////////////////////////////////////////////////
/*
 * Get the time since the last transition.
 */
template<typename SID,typename TID>
unsigned long SimpleFSM<SID,TID>::lastTransitioned() const {
  return (last_transition == 0) ? 0 : (millis() - last_transition);
}

/////////////////////////////////////////////////////////////////
/*
* Check if the FSM is finished.
*/
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::isFinished() const {
  return is_finished;
}

/////////////////////////////////////////////////////////////////
/*
* Run the FSM.
* interval: The interval in milliseconds.
* tick_cb: A callback function that is called on every tick.
*/
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::run(int interval /* = 1000 */, CallbackFunction tick_cb /* = NULL */) {
  unsigned long now = millis();
  // is the machine set up?
  if (!is_initialized) _initFSM();
  // are we ok?
  if (current_state == NULL) return;
  // is it time?
  if (!_isTimeForRun(now, interval)) return;
  // are we done yet?
  if (is_finished) return;
  // save the time
  last_run = now;
  // go through the timed events
  _handleTimedEvents(now);
  // trigger the on_state event
  // if (current_state->on_state != NULL) 
  current_state->on_state();
  // trigger the regular tick event
  // if (tick_cb != NULL) 
  tick_cb();
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::_isTimeForRun(unsigned long now, int interval) {
  return now >= last_run + interval;
}

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
void SimpleFSM<SID,TID>::_handleTimedEvents(unsigned long now) {
  for (int i = 0; i < num_timed; i++) {
    if (timed[i].from != current_state) continue;
    // start the transition timer 
    if (timed[i].start == 0) {
      timed[i].start = now;
      continue;
    }
    // reached the interval?
    if (now - timed[i].start >= timed[i].interval) {
      if (_transitionTo(&timed[i])) {
        timed[i].start = 0;
        return;
      }
    }
  }
}

/////////////////////////////////////////////////////////////////
/*
* Initialize the FSM.
*/
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::_initFSM() {
  if (is_initialized) return false;
  is_initialized = true;
  if (inital_state == NULL) return false;
  return _changeToState(inital_state, millis());
}

/////////////////////////////////////////////////////////////////
/*
 * Change to a new state.
 */
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::_changeToState(State<SID>* s, unsigned long now) {
  if (s == NULL) return false;
  // set the new state
  prev_state = current_state;
  current_state = s;
  // if (s->on_enter != NULL) 
  s->on_enter();
  // save the time
  last_run = now;
  last_transition = now;
  // is this the end?
  // if (s->is_final && finished_cb != NULL) finished_cb();
  on_transition_cb();
  if (s->is_final) finished_cb();
  if (s->is_final) is_finished = true;
  return true;
}

/////////////////////////////////////////////////////////////////
/*
 * Get the DOT definition of the FSM.
 */

// String SimpleFSM::getDotDefinition() {
//   return "digraph G {\n" + _dot_header() + dot_definition + _dot_active_node() + _dot_inital_state() + "}\n";
// }

/////////////////////////////////////////////////////////////////
template<typename SID,typename TID>
bool SimpleFSM<SID,TID>::_transitionTo(AbstractTransition<SID,TID>* transition) {
  // empty parameter?
  if (transition->to == NULL) return false;
  // can I pass the guard
  // if (transition->guard_cb != NULL && !transition->guard_cb()) return false;
  if(!transition->guard_cb()) return false;
  // trigger events
  // if (transition->from->on_exit != NULL) 
  transition->from->on_exit();
  //if (transition->on_run_cb != NULL) 
  transition->on_run_cb();
  // if (on_transition_cb != NULL) 
  

  return _changeToState(transition->to, millis());
}

/////////////////////////////////////////////////////////////////

// String SimpleFSM::_dot_transition(String from, String to, String label, String param) {
//   return "\t\"" + from + "\" -> \"" + to + "\"" + " [label=\"" + label + " (" + param + ")\"];\n";
// }

// /////////////////////////////////////////////////////////////////

// String SimpleFSM::_dot_inital_state() {
//   return inital_state ? "\t\"" + inital_state->getName() + "\" [style=filled fontcolor=white fillcolor=black];\n\n" : "";
// }

// /////////////////////////////////////////////////////////////////

// String SimpleFSM::_dot_active_node() {
//   return current_state ? "\t\"" + current_state->getName() + "\" [style=filled fontcolor=white];\n" : "";
// }

// /////////////////////////////////////////////////////////////////

// String SimpleFSM::_dot_header() {
//   return "\trankdir=LR; pad=0.5\n\tnode [shape=circle fixedsize=true width=1.5];\n";
// }

// /////////////////////////////////////////////////////////////////

// void SimpleFSM::_addDotTransition(Transition& t) {
//   dot_definition = dot_definition + _dot_transition(t.from->getName(), t.to->getName(), t.getName(), "ID=" + String(t.event_id));
// }

// /////////////////////////////////////////////////////////////////

// void SimpleFSM::_addDotTransition(TimedTransition& t) {
//   dot_definition = dot_definition + _dot_transition(t.from->getName(), t.to->getName(), t.getName(), String(t.getInterval()) + "ms");
// }

/////////////////////////////////////////////////////////////////
