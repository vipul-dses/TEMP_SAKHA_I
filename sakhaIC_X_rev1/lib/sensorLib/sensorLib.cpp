#include "Arduino.h"
#include "config.h"
#include "sensorLib.h"
#include "espCom.h"

int gasConc;
int regulatorMode;
float gasWeight;
float dGW = 14.2;
float cGW = 19.0;
float gP;
int gasPercentage;
float containerWeight;

void initializeSensor()
{
    pinMode(MQ6, INPUT);
    for (int i = 0; i < 60; i++)
    {
        unsigned long digitalReading = analogRead(MQ6);
        GAS_PRINTLN("Digital Reading: " + String(digitalReading));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    GAS_PRINTLN("Warm-up Complete");
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
    GAS_PRINTLN("Average Reading: " + String(averageReading));
    gasConc = round(averageReading * 10.0 / 4095.0);
    GAS_PRINTLN("Gas Concentration: " + String(gasConc) + " PPM");
}


float monitorCom()
{
  GAS_PRINTLN("Total Weight: " + String(totalWeight) + " KG");
  GAS_PRINTLN("Container Weight: " + String(containerWeight) + " KG");

  if (totalWeight >= 15.0)
  {
    float adjustment = (regulatorMode == 1) ? 0.5 : 0.0;

    if (containerWeight >= 15.0 && containerWeight < 18.0)
    {
      gasWeight = totalWeight - (containerWeight + adjustment);
      gP = (gasWeight / dGW) * 100;
    }
    else if (containerWeight >= 18.0 && containerWeight < 21.0)
    {
      gasWeight = totalWeight - (containerWeight + adjustment);
      gP = (gasWeight / cGW) * 100;
    }
    else
    {
      gasWeight = -1.0;
      gasPercentage = -1.0;
    }
  }
  else
  {
    gasWeight = -1.0;
    gasPercentage = -1.0;
  }
  gasPercentage = round(gP);

  GAS_PRINTLN("Gas Weight: " + String(gasWeight) + " KG");
  GAS_PRINTLN("Gas Percentage: " + String(gasPercentage) + " %");
  return gasWeight;
}