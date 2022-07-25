#include "Rotary_Shan.h"

Rotary_Shan::Rotary_Shan(uint8_t aPin, uint8_t bPin, uint8_t swPin, uint8_t activeLow)
    : aPin(aPin), bPin(bPin), swPin(swPin), activeLow(activeLow) {
    pinMode(aPin, INPUT);
    pinMode(bPin, INPUT);
    pinMode(swPin, INPUT);
    rotated = 0;
    pressed = 0;
}

void Rotary_Shan::attachRotationInterrupt(void (*func)()){
    attachInterrupt(digitalPinToInterrupt(aPin), func, FALLING);
}

void Rotary_Shan::detachRotationInterrupt(){
    detachInterrupt(digitalPinToInterrupt(aPin));
}

void Rotary_Shan::attachSwitchInterrupt(void (*func)()){
    attachInterrupt(digitalPinToInterrupt(swPin), func, FALLING);
}

void Rotary_Shan::detachSwitchInterrupt(){
    detachInterrupt(digitalPinToInterrupt(swPin));
}

void Rotary_Shan::handleSwitchState(){
    pressed = 1;
}


void Rotary_Shan::handleRotationCount(){
    rotated = 1;
    if(digitalRead(aPin) != digitalRead(bPin)){
        // cw rotation
        ++rotationCount;
    } else{
        // ccw rotation
        --rotationCount;
    }
}

int Rotary_Shan::getRotationCount(){
    return rotationCount;
}

void Rotary_Shan::resetCount(){
    rotationCount = 0;
    rotated = 0;
}

void Rotary_Shan::resetSwitchState(){
    pressed = 0;
}

uint8_t Rotary_Shan::isRotated(){
    return rotated;
}

uint8_t Rotary_Shan::isPressed_t(){
    return pressed;
}

uint8_t Rotary_Shan::isPressed(){
    if(pressed){
    	delay(20);
        if(!digitalRead(swPin) == activeLow){ // switch is pulled up i.e. grounded when pressed
            while(!digitalRead(swPin) == activeLow);
            digitalWrite(10, 1);
            delay(50);
            digitalWrite(10, 0);
            delay(50);
            return 1;
        }
    } 
    else if(!digitalRead(swPin) == activeLow){
    	delay(20);
        if(!digitalRead(swPin) == activeLow){ // switch is pulled up i.e. grounded when pressed
        	while(!digitalRead(swPin) == activeLow);
            digitalWrite(10, 1);
            delay(50);
            digitalWrite(10, 0);
            delay(50);
            return 1;
        }
    }
    return 0;
}

int Rotary_Shan::getValueInput(void (*onChange)(int newValue), int preValue, int min, int max,
    int8_t susceptibility, int8_t resolution, int8_t gradualChangeResolution, int8_t maxResolution){

    int value = preValue;
    if(gradualChangeResolution){
        resolution = map(value, min, max, maxResolution, 1);
    }

    while(!isPressed()){
        if(rotationCount > susceptibility){
            // value = (value+rotationCount) < max ? value + rotationCount : max;
            if((value+resolution) <= max){
                value += resolution;
            }
            resetCount();
        }
        if(rotationCount < -susceptibility){
            // value = (value+rotationCount) > min ? value + rotationCount : min;
            if((value-resolution) >= min){
                value -= resolution;
            }
            resetCount();
        }
        if(value != preValue){
            onChange(value);
            preValue = value;
            if(gradualChangeResolution){
                resolution = map(value, min, max, maxResolution, 1);
                if(!resolution)
                    resolution = 1;
            }
        }
    }
    pressed = 0;
    return value;
}

int Rotary_Shan::getValueInput(void (*onChange)(int newValue), void (*nonChange)(), int preValue, int min, int max,
    int8_t susceptibility, int8_t resolution, int8_t gradualChangeResolution, int8_t maxResolution){

    int value = preValue;
    if(gradualChangeResolution){
        resolution = map(value, min, max, maxResolution, 1);
    }
    
    while(!isPressed()){
        if(rotationCount > susceptibility){
            if((value+resolution) <= max){
                value += resolution;
            }
            resetCount();
        }
        if(rotationCount < -susceptibility){
            if((value-resolution) >= min){
                value -= resolution;
            }
            resetCount();
        }
        if(value != preValue){
            onChange(value);
            preValue = value;
            if(gradualChangeResolution){
                resolution = map(value, min, max, maxResolution, 0);
                if(!resolution)
                    resolution = 1;
            }
        }
        nonChange();
    }
    pressed = 0;
    return value;
}

void Rotary_Shan::getInputInfTime(void (*onChange)(int newValue), int preValue, int min, int max,
    int8_t susceptibility, int8_t resolution, int8_t gradualChangeResolution, int8_t maxResolution){

    int value = preValue;
    if(gradualChangeResolution){
        resolution = map(value, min, max, maxResolution, 1);
    }

    while(1){
        if(rotationCount > susceptibility){
            // value = (value+rotationCount) < max ? value + rotationCount : max;
            if((value+resolution) <= max){
                value += resolution;
            }
            resetCount();
        }
        if(rotationCount < -susceptibility){
            // value = (value+rotationCount) > min ? value + rotationCount : min;
            if((value-resolution) >= min){
                value -= resolution;
            }
            resetCount();
        }
        if(value != preValue){
            onChange(value);
            preValue = value;
            if(gradualChangeResolution){
                resolution = map(value, min, max, maxResolution, 1);
                if(!resolution)
                    resolution = 1;
            }
        }
    }
}

void Rotary_Shan::getInputInfTime(void (*onChange)(int newValue), void (*nonChange)(), int preValue, int min, int max,
    int8_t susceptibility, int8_t resolution, int8_t gradualChangeResolution, int8_t maxResolution){

    int value = preValue;
    if(gradualChangeResolution){
        resolution = map(value, min, max, maxResolution, 1);
    }
    
    while(1){
        if(rotationCount > susceptibility){
            if((value+resolution) <= max){
                value += resolution;
            }
            resetCount();
        }
        if(rotationCount < -susceptibility){
            if((value-resolution) >= min){
                value -= resolution;
            }
            resetCount();
        }
        if(value != preValue){
            onChange(value);
            preValue = value;
            if(gradualChangeResolution){
                resolution = map(value, min, max, maxResolution, 0);
                if(!resolution)
                    resolution = 1;
            }
        }
        nonChange();
    }
}

void Rotary_Shan::doUntilPressed(void (*func)()){
    while(!isPressed()){
        func();
    }
}


