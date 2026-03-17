#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>   

struct Pins {
    uint8_t inPin1 = 2;
    uint8_t inPin2 = 3;
    uint8_t motor1Out1 = 4;
    uint8_t motor1Out2 = 5;
    uint8_t motor2Out1 = 10;
    uint8_t motor2Out2 = 11;
    uint8_t ENA1 = 6;
    uint8_t ENA2 = 9;
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
