#include "StationTimer.hpp"

StationTimer::StationTimer(Ref<Component> parent) : 
    Component(parent),
    _timeout_callback([](){}),
    _timer_running(false), 
    _deadline(0),_fireImmediately(false),
    _period(0),
    _intervaling(false){
}

void StationTimer::setTimeout(VoidCallback callback, unsigned long delay) {
    cancel();
    this->_period = delay;
    this->_intervaling = false;
    this->_timer_running = false;
    this->_timeout_callback = callback;
    this->_fireImmediately=false;
}

void StationTimer::onInterval(VoidCallback callback, unsigned long interval,bool start,bool fireImmediately) {
    cancel();
    this->_fireImmediately=fireImmediately;
    this->_period = interval;
    this->_intervaling = true;

    this->_timer_running = start;
    this->_timeout_callback = callback;
    if(start){
        this->_deadline = millisTime() + interval;
    }
        
}

void StationTimer::start() {
    this->_timer_running = true;
    if(this->_intervaling && this->_fireImmediately){
        this->_timeout_callback();
    }
    this->_deadline = millisTime() + this->_period;  
}

void StationTimer::reset() {
    this->_period=0ul;
    this->_intervaling=false;
    this->_deadline=0ul;
}

void StationTimer::cancel() {
    this->_timer_running = false;
}

bool StationTimer::isRunning() {
    return this->_timer_running;
}

unsigned long StationTimer::timeLeft() {
    return this->_deadline - millis();
}

void StationTimer::privateLoop() {
    if(!this->_timer_running) return;
    if(millisTime() >= this->_deadline) {
        if(this->_intervaling) {
            this->_deadline = millis() + this-> _period;
            this->_timeout_callback();
        } else {
            this->cancel();
            this->_timeout_callback();
        }
    }
}


/* 

TimerOneShot::TimerOneShot(Ref<Component> parent) : 
    Component(parent),
    _timeout_callback([](){}),
    _timer_running(false), 
    _deadline(0) {
}

void TimerOneShot::setTimeout(VoidCallback callback, unsigned long delay) {
    cancel();
    _timer_running = false;
    _timeout_callback = callback;
    this->_delay=delay;
}

void TimerOneShot::Start() {
    this->_timer_running = true;
    this->_deadline = millisTime() + this->_delay;
}

void TimerOneShot::Reset() {
    this->_deadline = millisTime() + this->_delay;
}

void TimerOneShot::cancel() {
    _timer_running = false;
}

bool TimerOneShot::isRunning() {
    return _timer_running;
}

unsigned long TimerOneShot::timeLeft() {
    return _deadline - millisTime();
}

void TimerOneShot::privateLoop() {
    if(!_timer_running) return;
    if(millisTime() >= _deadline) {
        cancel();
        _timeout_callback();
    }
} */