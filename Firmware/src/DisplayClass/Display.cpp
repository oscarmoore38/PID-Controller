#include "Display.h"

Display::Display()
    : oledDisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1)
    {};


void Display:: Begin(){
    Wire.begin();
    oledDisplay.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    oledDisplay.clearDisplay();
    oledDisplay.setTextSize(2);
    oledDisplay.setTextColor(SSD1306_WHITE);
    oledDisplay.display();
}

void Display:: displayMotorRPM(const float Motor1RPM, const float Motor2RPM){
    oledDisplay.clearDisplay(); 
    oledDisplay.setCursor(0, 0);
    oledDisplay.print("Motor1 RPM:");
    oledDisplay.setCursor(0, 16);
    oledDisplay.print(Motor1RPM, 1);
    oledDisplay.setCursor(0, 32);
    oledDisplay.print("Motor2 RPM:");
    oledDisplay.setCursor(0, 48);
    oledDisplay.print(Motor2RPM, 1);
    oledDisplay.display();
}


