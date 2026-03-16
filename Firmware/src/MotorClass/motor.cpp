#include "motor.h"
#include <stdint.h>

Motor:: Motor(const MotorConfig& myMotor):  motorRPM(0), PWM(myMotor.defaultPWM), RPMSetPoint(myMotor.RPMSetPoint), motorPPR(myMotor.ppr) {}

uint16_t Motor:: getMotorPPR() const{
    return motorPPR; 
}