#include "PID.h"

PIDController:: PIDController(float KpVal, float KiVal){
    Kp = KpVal;
    Ki = KiVal;
    Integral = 0;  
}

float PIDController:: PIDControl(float error, Motor& myMotor, LoopTime& myTime)
{
    error = myMotor.RpmSetPoint - myMotor.RPMs;

    if((myMotor.PWM <= 0 && error < 0) || (myMotor.PWM >= 255 && error > 0)){
        myMotor.PWM += (Kp * error);  
    } else{
        Integral += error * myTime.dt; 
        myMotor.PWM += (Kp * error) + (Ki * Integral);  
    }

    if (myMotor.PWM < 0){
        myMotor.PWM = 0; 
    } else if (myMotor.PWM > 255){
        myMotor.PWM = 255; 
    }

    return myMotor.PWM;
};
