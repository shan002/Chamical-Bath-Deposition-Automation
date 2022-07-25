#include "timer.h"

Timer::Timer(unsigned long gapMillis, void (*callBackFunc)(unsigned long passedTime))
	: gapMillis(gapMillis), callBackFunc(callBackFunc){
	isTimerOn = 0;
}

void Timer::setTimerOn(){
	isTimerOn = 1;
	startMillis = millis();
}

void Timer::setTimerOff(){
	isTimerOn = 0;
}

void Timer::setGapTime(unsigned long timeMillis){
	gapMillis = timeMillis;
}

void Timer::setCallBackFunc(void (*callBackFunc)(unsigned long passedTime)){
	this->callBackFunc = callBackFunc;
}

void Timer::run(){
	if(!isTimerOn){
		return;
	}
    unsigned long passedTime = millis() - startMillis;
	if(passedTime > gapMillis){
		callBackFunc(passedTime);
		startMillis = millis();
	}
}
