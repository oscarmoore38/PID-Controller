#ifndef Display_h
#define Display_h

#include "Wire.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <string.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C


class Display {
public: 
    Display();
    void Begin();

    template<typename T>
    void displayOLED(const T&  valueToDisplay){
        oledDisplay.clearDisplay();
        oledDisplay.setCursor(0,0);
        oledDisplay.print(valueToDisplay);   
        oledDisplay.display();
    }

    void displayMotorRPM(const float Motor1RPM, const float Motor2RPM);

private: 
    Adafruit_SSD1306 oledDisplay;
};

#endif