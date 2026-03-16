#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h> 

struct MotorConfig{
    uint16_t ppr; 
    uint16_t defaultPWM; 
    uint8_t RPMSetPoint; 
};

class Motor{
public: 
    Motor(const MotorConfig& myMotor);
    uint16_t getMotorPPR() const; 

    float motorRPM; 
    uint16_t PWM; 
    uint8_t RPMSetPoint;

private: 
    const uint16_t motorPPR; 
};

#endif