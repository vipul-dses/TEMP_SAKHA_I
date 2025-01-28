#include "Arduino.h"
#include "config.h"
#include "sensorLib.h"

int gasConc;

void initializeSensor()
{
    pinMode(MQ6, INPUT);
    SERIAL_PRINTLN("Warm-up Start");
    for (int i = 0; i < 60; i++)
    {
        unsigned long initialReading = analogRead(MQ6);
        SERIAL_PRINTLN("Initial Reading: " + String(initialReading));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    SERIAL_PRINTLN("Warm-up Complete");
}

void monitorSensor()
{
    unsigned long digitalReading = 0;
    for (int i = 0; i < 500; i++)
    {
        digitalReading += analogRead(MQ6);
        delayMicroseconds(10);
    }
    int averageReading = digitalReading / 500;
    SERIAL_PRINTLN("Average Reading: " + String(averageReading));
    gasConc = round(averageReading * 10.0 / 4095.0);
    SERIAL_PRINTLN("Gas Concentration: " + String(gasConc) + " PPM");
}