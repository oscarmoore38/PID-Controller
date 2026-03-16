#include "Arduino.h"
#include "Wire.h"
#include "DisplayClass/Display.h"
#include "PID/PID.h"
#include "Types.h"
#include "MotorClass/motor.h"

constexpr MotorConfig MOTOR1_CONFIG = {408, 127, 150};
constexpr MotorConfig MOTOR2_CONFIG = {408,  64, 100};

Display myDisplay; 
PIDController PIDControllerMotor1(0.5, 0.05); 
PIDController PIDControllerMotor2(0.5, 0.05); 
Motor motor1(MOTOR1_CONFIG); 
Motor motor2(MOTOR2_CONFIG); 

LoopTime myTime;
Pins myPins; 

volatile int pulseCount1 = 0; 
volatile int pulseCount2 = 0; 

// ISR 
void IsrIncrementPulseCount1(){
  pulseCount1 ++; 
}

void IsrIncrementPulseCount2(){
  pulseCount2 ++; 
}

void setup()
{
  pinMode(myPins.inPin1, INPUT);
  pinMode(myPins.inPin2, INPUT);
  pinMode(myPins.motor1Out1, OUTPUT);
  pinMode(myPins.motor1Out2, OUTPUT);
  pinMode(myPins.motor2Out1, OUTPUT);
  pinMode(myPins.motor2Out2, OUTPUT);
  pinMode(myPins.ENA1, OUTPUT);
  pinMode(myPins.ENA2, OUTPUT);

  digitalWrite(myPins.ENA1, LOW);
  digitalWrite(myPins.ENA2, LOW);
  
  digitalWrite(myPins.motor1Out1, HIGH);
  digitalWrite(myPins.motor1Out2, LOW);

  digitalWrite(myPins.motor2Out1, HIGH);
  digitalWrite(myPins.motor2Out2, LOW);

  analogWrite(myPins.ENA1, motor1.PWM);
  analogWrite(myPins.ENA2, motor2.PWM);

  Serial.begin(115200);

  myDisplay.Begin();

  attachInterrupt(digitalPinToInterrupt(myPins.inPin1), IsrIncrementPulseCount1, RISING);
  attachInterrupt(digitalPinToInterrupt(myPins.inPin2), IsrIncrementPulseCount2, RISING);
}

void loop()
{

  myTime.currentTime = millis(); 

  // Sample rate is the same for both 
  if (myTime.currentTime - myTime.lastTimeMotorOutput >= myTime.MotorOutputTimeCheck){
    noInterrupts(); 
    int pulseCount1Local = pulseCount1; 
    int pulseCount2Local = pulseCount2;
    pulseCount1 = 0; 
    pulseCount2 = 0; 
    interrupts(); 

    myTime.dt = (myTime.currentTime - myTime.lastTimeMotorOutput) / 1000.0f; 
    
    motor1.motorRPM = ((float)pulseCount1Local/motor1.getMotorPPR()) * (60.0f/myTime.dt);
    motor2.motorRPM = ((float)pulseCount2Local/motor2.getMotorPPR()) * (60.0f/myTime.dt);
    
    motor1.PWM = PIDControllerMotor1.PIDControl(motor1, myTime);
    motor2.PWM = PIDControllerMotor2.PIDControl(motor2, myTime);

    analogWrite(myPins.ENA1, motor1.PWM);
    analogWrite(myPins.ENA2, motor2.PWM);
    
    myTime.lastTimeMotorOutput += myTime.MotorOutputTimeCheck; 

  }

  if (myTime.currentTime - myTime.lastTimeDisplayOutput >= myTime.DisplayOutputTimeCheck){
    myDisplay.displayMotorRPM(motor1.motorRPM, motor2.motorRPM);

    myTime.lastTimeDisplayOutput += myTime.DisplayOutputTimeCheck;
  }

  // if (myTime.currentTime - myTime.lastTimeSerialOutput >= myTime.SerialOutputTimeCheck){
  //   Serial.print(myTime.currentTime);
  //   Serial.print(",");
  //   Serial.print(motor1.motorRPM);
  //   Serial.print(",");
  //   Serial.println(motor1.PWM);

  //   myTime.lastTimeSerialOutput += myTime.SerialOutputTimeCheck;
  // }

}

