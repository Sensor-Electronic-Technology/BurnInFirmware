#pragma once
#include "../constants.h"


struct BurnTimerConfig{
	unsigned long time60mASecs=TIME_SECS_60mA;
	unsigned long time120mASecs=TIME_SECS_120mA;
	unsigned long time150mASecs=TIME_SECS_150mA;
	unsigned long timeOffPercent=TIME_OFF_PERCENT;
	void Reset(){
		this->time60mASecs=TIME_SECS_60mA;
		this->time120mASecs=TIME_SECS_120mA;
		this->time150mASecs=TIME_SECS_150mA;
		this->timeOffPercent=TIME_OFF_PERCENT;
	}
};