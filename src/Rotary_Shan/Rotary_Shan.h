#ifndef _ROTARY_SHAN_H_
#define _ROTARY_SHAN_H_
#include <Arduino.h>
#include <limits.h>

class Rotary_Shan{
private:
    uint8_t aPin, bPin, swPin; // interrupt will be connected to aPin
    uint8_t activeLow;
    volatile int rotationCount;
    volatile uint8_t rotated, pressed;
public:
    Rotary_Shan(uint8_t aPin, uint8_t bPin, uint8_t swPin, uint8_t activeLow = 1);
    void attachRotationInterrupt(void (*func)());
    void detachRotationInterrupt();
    void attachSwitchInterrupt(void (*func)());
    void detachSwitchInterrupt();
    void handleRotationCount();  // rotation ISR in the main function will call this function of that instance
    void handleSwitchState();  // switch ISR in the main function will call this function of that instance
    int getRotationCount();
    void resetCount();
    void resetSwitchState();
    int getValueInput(void (*onChange)(int newValue), int preValue=0, int min = INT_MIN, int max = INT_MAX, int8_t susceptibility = 0, int8_t resolution=1, int8_t gradualChangeResolution=0, int8_t maxResolution=1);
    int getValueInput(void (*onChange)(int newValue), void (*nonChange)(), int preValue=0, int min = INT_MIN, int max = INT_MAX, int8_t susceptibility = 0, int8_t resolution=1, int8_t gradualChangeResolution=0, int8_t maxResolution=1);
    void getInputInfTime(void (*onChange)(int newValue), int preValue=0, int min = INT_MIN, int max = INT_MAX, int8_t susceptibility = 0, int8_t resolution=1, int8_t gradualChangeResolution=0, int8_t maxResolution=1);
    void getInputInfTime(void (*onChange)(int newValue), void (*nonChange)(), int preValue=0, int min = INT_MIN, int max = INT_MAX, int8_t susceptibility = 0, int8_t resolution=1, int8_t gradualChangeResolution=0, int8_t maxResolution=1);
    void doUntilPressed(void (*func)());
    uint8_t isRotated();
    uint8_t isPressed();
    uint8_t Rotary_Shan::isPressed_t();
};

#endif
