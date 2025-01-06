#include "Arduino.h"
#include "config.h"
#include "ledLib.h"

void initializeLed()
{
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
}

void setColor(int red, int green, int blue)
{
    analogWrite(RED, red);
    analogWrite(GREEN, green);
    analogWrite(BLUE, blue);
}

void greenColor()
{
    //setColor(0, 255, 0);
    setColor(255, 0, 255);  //CA
   // SERIAL_PRINTLN("Green color activated");
}

void orangeColor()
{
    //setColor(255, 25, 0);
    setColor(0, 225, 255);  //CA
    SERIAL_PRINTLN("Orange color activated");
}

void redColor()
{
   // setColor(255, 0, 0);
    setColor(0, 255, 255);  //CA
    SERIAL_PRINTLN("Red color activated");
}

void yellowColor()
{
    //setColor(255, 255, 0);
    setColor(0, 0, 255);  //CA
    SERIAL_PRINTLN("Yellow color activated");
}

void whiteColor()
{
    //setColor(255, 255, 255);
    setColor(25, 0, 0);  //CA
   // SERIAL_PRINTLN("White color activated");
}

void lightColor()
{
    //setColor(0, 255, 25);
    setColor(255, 0, 225);  //CA
    SERIAL_PRINTLN("Light color activated");
}

void aquaColor()
{
    //setColor(0, 255, 255);
    setColor(255, 0, 0);  //CA
    SERIAL_PRINTLN("Aqua color activated");
}

void blueColor()
{
    //setColor(0, 0, 255);
    setColor(255, 255, 0);  //CA
    SERIAL_PRINTLN("Blue color activated");
}
