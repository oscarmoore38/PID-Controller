#ifndef PID_h 
#define PID_h

#include "Types.h"
#include "MotorClass/motor.h"

class PIDController{
public:
    PIDController(float KpVal, float kiVal); 
    float PIDControl(Motor& motorObj, LoopTime& myTime);  

private: 
    float Integral;
    float Kp; 
    float Ki; 
};


#endif