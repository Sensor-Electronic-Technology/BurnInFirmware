#pragma once
#include "constants.h"
#include <ArduinoComponents.h>

using namespace components;

class StationTimer : public Component {
public:
	StationTimer(Ref<Component> parent);
	StationTimer() : StationTimer(nullptr) {}
	void setTimeout(VoidCallback callback, unsigned long delay);
	void onInterval(VoidCallback callback, unsigned long interval,bool start=true);
	void changeCallback(VoidCallback callback){
		this->_timeout_callback = callback;
	}
	bool isRunning();
	unsigned long timeLeft();
	void cancel();
	void start();
	void reset();

private:
	void privateLoop();

	VoidCallback _timeout_callback;
	unsigned long _deadline;
	bool _timer_running;
	unsigned _period;
	bool _intervaling;
};

/* class TimerOneShot : public Component {
public:
    TimerOneShot(Ref<Component> parent);
    TimerOneShot() : TimerOneShot(nullptr) {}
    void setTimeout(VoidCallback callback, unsigned long delay);
    bool isRunning();
    void Start();
    void Reset();
    unsigned long timeLeft();
    void cancel();

private:
    void privateLoop();
    VoidCallback _timeout_callback;
    unsigned long _deadline;
    unsigned long _delay;
    bool _timer_running;
    
}; */


/* 	class TimerRecurring : public Component {
	public:
		TimerRecurring(Ref<Component> parent);
		TimerRecurring() : TimerRecurring(nullptr) {}
		void onInterval(VoidCallback callback, unsigned long interval);
		bool isRunning();
		unsigned long timeLeft();
		void cancel();
        void start();

	private:
		void privateLoop();

		VoidCallback _timeout_callback;
		unsigned long _deadline;
		bool _timer_running;
		
		unsigned _interval;
		bool _intervaling;
	}; */


