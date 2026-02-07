
#include "Arduino.h"
#include "Wire.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Globals / volatile variables 
volatile int pulseCount = 0; 
int inPin1 = 2; 
int inPin2 = 3; 
int motorOut1 = 4; 
int motorOut2 = 5; 
int ENA = 6; 
int PWM = 127; 
float RPMs = 0; 
float dt = 0; 
const int motorPPR = 408; 
const float RpmSetPoint = 150; 
float error = 0; 
float Integral = 0; 
float Kp = 0.5;
float Ki = 0.07; 

// Function prototypes 
void UpdateRPMs(int pulses, float dt_sec); 
void Display();
void PIDControl(); 

// ISR 
void IsrIncrement(){
  pulseCount ++; 
}

void setup()
{

  pinMode(inPin1, INPUT);
  pinMode(inPin2, INPUT);
  pinMode(motorOut1, OUTPUT);
  pinMode(motorOut2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Set ENA pin to low explicitly
  digitalWrite(ENA, LOW);
  
  // Set direction
  digitalWrite(motorOut1, HIGH);
  digitalWrite(motorOut2, LOW);

  // Set full power. Will use analog for custom speeds later. 
  analogWrite(ENA, PWM);

  attachInterrupt(digitalPinToInterrupt(2), IsrIncrement, RISING);
  // attachInterrupt(digitalPinToInterrupt(3), IsrPin3, CHANGE);

  Wire.begin();
  // Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.display();

}

void loop()
{

  static unsigned long lastTime = 0; 
  unsigned long currentTime = millis(); 

  if (currentTime - lastTime >= 500){
    noInterrupts(); 
    int pulseCountLocal = pulseCount; 
    pulseCount = 0; 
    interrupts(); 

    dt = (currentTime - lastTime) / 1000.0f; 

    UpdateRPMs(pulseCountLocal, dt);

    Display();

    lastTime = currentTime;

    PIDControl(); 

  }

}


void UpdateRPMs(int pulses, float dt_sec){
  RPMs = ((float)pulses/motorPPR) * (60.0f/dt_sec);
}

void Display(){
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("RPM:");
   display.setCursor(0,24);
   display.print(RPMs, 1);   
   display.display();
}

void PIDControl(){
  error = RpmSetPoint - RPMs;

  if((PWM <= 0 && error < 0) || (PWM >= 255 && error > 0)){
    PWM += (Kp * error);  
  } else{
    Integral += error * dt; 
    PWM += (Kp * error) + (Ki * error * dt);  
  }

  if (PWM < 0){
    PWM = 0; 
  } else if (PWM > 255){
    PWM = 255; 
  }

  analogWrite(ENA, PWM);

}