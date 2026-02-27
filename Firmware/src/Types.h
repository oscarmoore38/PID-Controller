#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>   

struct Pins {
    uint8_t inPin1 = 2;
    uint8_t inPin2 = 3;
    uint8_t motorOut1 = 4;
    uint8_t motorOut2 = 5;
    uint8_t ENA = 6;
};

struct Motor {
    int PWM = 127;                 
    float RPMs = 0;
    const uint16_t motorPPR = 408;
    const float RpmSetPoint = 150;
};

struct LoopTime {
    unsigned long lastTimeMotorOutput = 0;
    unsigned long lastTimeDisplayOutput = 0;
    unsigned long lastTimeSerialOutput = 0;
    unsigned long currentTime = 0;
    float dt = 0;
    const unsigned long MotorOutputTimeCheck = 80; 
    const unsigned long DisplayOutputTimeCheck = 400; 
    const unsigned long SerialOutputTimeCheck = 500;  
};

#endif
