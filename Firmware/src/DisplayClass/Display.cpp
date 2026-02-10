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


