#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "Fonts/FreeSerifBold24pt7b.h"
#include "Switch.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET 5
#define SCREEN_ADDRESS 0x3D

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define BUTTON_PIN 2
#define IRON_RELAY 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Switch button(BUTTON_PIN);

unsigned long lastIntervalTime = 0;
int x = 0;
bool updateDisplay = false;
bool active = false;

void turnOn(void* p) {
    if (active) return;

    x = 320;
    active = true;
    lastIntervalTime = millis();
    updateDisplay = true;
}

void turnOff(void* p) {
    if (!active) return;

    x = 0;
    active = false;
    display.clearDisplay();
    display.display();
}

void increaseTimer(void *p) {
    if (!active) return;

    x += 320;
    lastIntervalTime = millis();
    updateDisplay = true;
}

void setup() {
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(IRON_RELAY, OUTPUT);
    digitalWrite(IRON_RELAY, LOW);

    Serial.begin(9600);
    Wire.begin();

    button.setPushedCallback(&turnOn);
    button.setLongPressCallback(&turnOff);
    button.setDoubleClickCallback(&increaseTimer);

    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        while (true) {}
    } else {
        Serial.println("ssd1306 ready");
    }

    display.clearDisplay();
    display.display();
}


void loop() {
    button.poll();

    // count down every second, turn off when we reach 0
    if (active && (lastIntervalTime + 1000) < millis()) {
        x--;
        updateDisplay = true;
        lastIntervalTime = millis();

        if (x == 0) {
            active = false;
            display.clearDisplay();
            display.display();
        }
    }

    // update the display
    if (updateDisplay) {
        display.clearDisplay();
        display.setCursor(0, 30);
        
        display.setFont(&FreeSerifBold24pt7b);
        display.setTextColor(SSD1306_WHITE);

        char buf[3];
        sprintf(buf, "%d", x);

        int16_t x1;
        int16_t y1;
        uint16_t width;
        uint16_t height;
        display.getTextBounds(buf, 0, 0, &x1, &y1, &width, &height);

        display.clearDisplay(); // clear display
        display.setCursor((SCREEN_WIDTH - width) / 2, height + 18);
        display.println(buf); // text to display
        display.display();
        updateDisplay = false;
    }

    // ensure the iron is on if we're active
    if (active) {
        digitalWrite(IRON_RELAY, HIGH);
    } else {
        digitalWrite(IRON_RELAY, LOW);
    }
}