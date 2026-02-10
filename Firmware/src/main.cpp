#include "Arduino.h"
#include "Wire.h"
#include "DisplayClass/Display.h"
#include "PID/PID.h"
#include "Types.h"

Display myDisplay; 
PIDController myPIDController(0.5, 0.07); 

LoopTime myTime;
Motor myMotor;
Pins myPins; 

volatile int pulseCount = 0; 
float error = 0; 

// ISR 
void IsrIncrement(){
  pulseCount ++; 
}

void setup()
{
  pinMode(myPins.inPin1, INPUT);
  pinMode(myPins.inPin2, INPUT);
  pinMode(myPins.motorOut1, OUTPUT);
  pinMode(myPins.motorOut2, OUTPUT);
  pinMode(myPins.ENA, OUTPUT);

  digitalWrite(myPins.ENA, LOW);
  
  digitalWrite(myPins.motorOut1, HIGH);
  digitalWrite(myPins.motorOut2, LOW);

  analogWrite(myPins.ENA, myMotor.PWM);

  myDisplay.Begin();

  attachInterrupt(digitalPinToInterrupt(myPins.inPin1), IsrIncrement, RISING);
}

void loop()
{

  myTime.currentTime = millis(); 

  if (myTime.currentTime - myTime.lastTime >= 500){
    noInterrupts(); 
    int pulseCountLocal = pulseCount; 
    pulseCount = 0; 
    interrupts(); 

    myTime.dt = (myTime.currentTime - myTime.lastTime) / 1000.0f; 

    myMotor.RPMs = ((float)pulseCountLocal/myMotor.motorPPR) * (60.0f/myTime.dt);

    myDisplay.displayOLED(myMotor.RPMs);

    myTime.lastTime = myTime.currentTime;

    myMotor.PWM = myPIDController.PIDControl(error, myMotor, myTime);

    analogWrite(myPins.ENA, myMotor.PWM);

  }

}

