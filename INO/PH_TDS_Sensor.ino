#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <Adafruit_SH110X.h>
#include <splash.h>

#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3c
#define OLED_RESET -1 

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define PH_PIN A1

float voltage, phValue, temperature = 25;
DFRobot_PH ph;

int buttonPin = 8;  // the number of the first pushbutton pin
int buttonPin2 = 9; // the number of the second pushbutton pin
int buttonPin3 = 10; // the number of the LED pin

int buttonState = LOW;            
int buttonState2 = LOW;
int buttonState3 = LOW;
     
int lastButtonState = LOW;  
int lastButtonState2 = LOW; 
int lastButtonState3 = LOW; 

bool wasButtonPressed = false;
bool wasButton2Pressed = false;
bool wasButton3Pressed = false;

unsigned long lastDebounceTime = 0;  
unsigned long lastDebounceTime2 = 0;
unsigned long lastDebounceTime3 = 0;

unsigned long debounceDelay = 50;
float lastPhValue = -1.0;  // To track the last displayed pH value

void setup() {
    Serial.begin(115200);  
    delay(500);
    ph.begin();

    display.begin(SCREEN_ADDRESS, true); 
    display.clearDisplay();
  
    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);
}

void loop() {
    static unsigned long timepoint = millis();
    if (millis() - timepoint > 2000U) { // time interval: 1s
        timepoint = millis();
        voltage = analogRead(PH_PIN) / 1024.0 * 5000; // read the voltage
        phValue = ph.readPH(voltage, temperature); // convert voltage to pH with temperature compensation
        if(phValue != lastPhValue) {
            //phondisplay(phValue);
            Serial.println(phValue);
            lastPhValue = phValue;
        }
        //Serial.println(phValue);
        //phondisplay(phValue);

    }
    ph.calibration(voltage, temperature); // calibration process by Serial CMD
    button();
}

void phondisplay(float Test) {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println(Test);
    display.display();
}

void calondisplay(float result) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    Serial.println("result: ");
    Serial.print(result);

    if(result == 1){
      display.println("Kalibration gestartet");
    } else if(result == 2){
      display.println("PH Loesung 7");
    } else if(result == 3){
      display.println("PH Loesung 4");
    } else if(result == 4 || result == 6){
      display.println("Error");
    } else if(result == 5){
      display.println("saved");
    }

    display.display();
    delay(3000);
}

void button() {
    int result;
    int reading = digitalRead(buttonPin);
    int reading2 = digitalRead(buttonPin2);
    int reading3 = digitalRead(buttonPin3);

    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }

    if (reading2 != lastButtonState2) {
        lastDebounceTime2 = millis();
    }

    if (reading3 != lastButtonState3) {
        lastDebounceTime3 = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == LOW && !wasButtonPressed) {
                result = ph.startCalibration(1);
                Serial.println("Button1 gedruckt");
                //calondisplay(result);
                wasButtonPressed = true;
            }
        } else {
            wasButtonPressed = false;
        }
    }

    if ((millis() - lastDebounceTime2) > debounceDelay) {
        if (reading2 != buttonState2) {
            buttonState2 = reading2;
            if (buttonState2 == LOW && !wasButton2Pressed) {
                result = ph.startCalibration(2);
                Serial.println("Button2 gedruckt");
                //calondisplay(result);
                wasButton2Pressed = true;
            }
        } else {
            wasButton2Pressed = false;
        }        
    }

    if ((millis() - lastDebounceTime3) > debounceDelay) {
        if (reading3 != buttonState3) {
            buttonState3 = reading3;
            if (buttonState3 == LOW && !wasButton3Pressed) {
                result = ph.startCalibration(3);
                Serial.println("Button3 gedruckt");
                //calondisplay(result);
                wasButton3Pressed = true;
            }
        } else {
            wasButton3Pressed = false;
        }
    }

    lastButtonState = reading;
    lastButtonState2 = reading2;
    lastButtonState3 = reading3;
}
