#ifndef _TIMER_H_
#define _TIMER_H_

#include <Arduino.h>

class Timer{

private:
	bool isTimerOn;
	unsigned long startMillis;
	unsigned long gapMillis;
	void (*callBackFunc)(unsigned long passedTime);
public:
	Timer(unsigned long gapTime, void (*callBackFunc)(unsigned long passedTime));
	void setGapTime(unsigned long gapTime);
	void setCallBackFunc(void (*callBackFunc)(unsigned long passedTime));
	void setTimerOn();
	void setTimerOff();
	void run();
};

#endif
