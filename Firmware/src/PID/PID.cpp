#include "PID.h"

PIDController:: PIDController(float KpVal, float KiVal){
    Kp = KpVal;
    Ki = KiVal;
    Integral = 0;  
}

float PIDController:: PIDControl(Motor& motorObj, LoopTime& myTime)
{
    float error = motorObj.RPMSetPoint - motorObj.motorRPM;

    if((motorObj.PWM <= 0 && error < 0) || (motorObj.PWM >= 255 && error > 0)){
        motorObj.PWM += (Kp * error);  
    } else{
        Integral += error * myTime.dt; 
        motorObj.PWM += (Kp * error) + (Ki * Integral);  
    }

    if (motorObj.PWM < 0){
        motorObj.PWM = 0; 
    } else if (motorObj.PWM > 255){
        motorObj.PWM = 255; 
    }

    return motorObj.PWM;
};
