#include "Arduino.h"
#include "config.h"
#include "ledLib.h"
#include "pinDefs.h"

void initializeLed()
{
    pinMode(REDLED, OUTPUT);
    pinMode(GRNLED, OUTPUT);
    pinMode(BLULED, OUTPUT);   
    // digitalWrite(REDLED, HIGH);
    // digitalWrite(GRNLED, HIGH);
    // digitalWrite(BLULED, LOW);
    analogWrite(REDLED, 0);
    analogWrite(GRNLED, 0);
    analogWrite(BLULED, 0);
}

void setColor(int red, int green, int blue)
{
    analogWrite(REDLED, red);
    analogWrite(GRNLED, green);
    analogWrite(BLULED, blue);
}

void greenColor()
{
    setColor(255, 0, 255);
    Serial.println("Green color activated");
}

void orangeColor()
{
    setColor(0, 235, 255);
    Serial.println("Orange color activated");
}

void redColor()
{
    setColor(0, 255, 255);
    Serial.println("Red color activated");
}

void yellowColor()
{
    setColor(0, 0, 255);
    Serial.println("Yellow color activated");
}
void whiteColor()
{
    setColor(0, 0, 0);
    Serial.println("white color activated");
}
void lightColor()
{
    setColor(0, 255, 25);
    SERIAL_PRINTLN("Light color activated");
}

void aquaColor()
{
    setColor(255, 0, 0);
    SERIAL_PRINTLN("Aqua color activated");
}

void blueColor()
{
    setColor(255, 255, 0);
    SERIAL_PRINTLN("Blue color activated");
}