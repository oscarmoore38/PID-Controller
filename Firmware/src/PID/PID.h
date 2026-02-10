#ifndef PID_h 
#define PID_h

#include "Types.h"

class PIDController{
public:
    PIDController(float KpVal, float kiVal); 
    float PIDControl(float error, Motor& myMotor, LoopTime& myTime);  

private: 
    float Integral;
    float Kp; 
    float Ki; 
};


#endif