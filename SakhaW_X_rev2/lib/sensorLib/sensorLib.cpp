#include "sensorLib.h"
#include "Arduino.h"
#include "HX711.h"
#include "pinDefs.h"
#include "config.h"

HX711 scale;

const float calibrationFactor = 66903.18; // 111171.11; // Sakha-I 1variant ///home unit 66903.18
const long zeroFactor = 167308.6;         // Replace with your zero factor 150941 //home unit 167308.6
float weight = 0.0;
int regulatorMode;
// float containerWeight;
float dGW = 14.2;
float cGW = 19.0;
float gasWeight;
float gP;
int gasPercentage;
bool sensorFlag = false;
extern float containerWeight;
float incomingCW; // only for now delete after including BLE & WIFI functionality
float readSensor()
{
  // calibrationFactor =66903.18;
  // zeroFactor = 167308.6 ;
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibrationFactor);
  // WEIGHT_PRINT("Calibration factor: ");
  // WEIGHT_PRINTLN(String(calibrationFactor));
  scale.set_offset(zeroFactor);
  // WEIGHT_PRINT("Zero factor: ");
  // WEIGHT_PRINTLN(String(zeroFactor));
  weight = (scale.get_units(10));
  sensorFlag = true;
  // WEIGHT_PRINT("flag status");
  // WEIGHT_PRINTLN(String(sensorFlag));
  WEIGHT_PRINT("Actual readSensor weight: ");
  WEIGHT_PRINTLN(String(weight) + " KG");
  //weight = 20.20; // for testing gas % below 1%

  if (containerWeight >= 15.0)
  {
    WEIGHT_PRINTLN("Gas Weight loop: " + String(weight) + " KG");

    float adjustment = (regulatorMode == 1) ? 0.5 : 0.0;
    gasWeight = weight - (containerWeight + adjustment);
    WEIGHT_PRINTLN("container Weight loop: " + String(containerWeight) + " KG");
    if (containerWeight >= 15.0 && containerWeight < 18.0)
    {
      gasWeight = weight - (containerWeight + adjustment);
      gP = (gasWeight / dGW) * 100;
      gasWeight = weight - (containerWeight + adjustment);
      WEIGHT_PRINTLN("Gas Weight: " + String(gasWeight) + " KG");
      if (gasWeight >= 0.02 && gasWeight <= 0.2)
      {
        gP = 1;
      }
      else if (gasWeight < 0.02 && gasWeight >= 0)
      {
        gP = 0;
      }
      else
      {
        gP = (gasWeight / dGW) * 100;
      }
    }
    else if (containerWeight >= 18.0 && containerWeight < 21.0)
    {
      gasWeight = weight - (containerWeight + adjustment);
      gP = (gasWeight / cGW) * 100;
    }
    else
    {
      gasWeight = -1.0;
      gasPercentage = -1.0;
    }
  }

  gasPercentage = round(gP);
  if (gasPercentage >= 100)
  {
    gasPercentage = 100;
  }
  WEIGHT_PRINTLN("Gas Weight: " + String(gasWeight) + " KG");
  WEIGHT_PRINTLN("Gas Percentage: " + String(gasPercentage) + " %");

  return gasWeight;
}
/**
 * @brief hx711 Power down
 *
 * This function does not return.
 */
void sleepSensor()
{
  WEIGHT_PRINTLN("Sensor Power Down");
  scale.power_down();
  delay(10);
}

float getBatteryPercentage()
{

  // Constants for battery voltage levels
  const float fullChargeVoltage = 3.45;  // Full charge voltage
  const float dischargeVoltage = 2.65;   // Discharge voltage
  const float maxADCValue = 4095.0;      // Max value of the ADC (12-bit resolution)
  const float adcReferenceVoltage = 3.3; // ESP32 ADC reference voltage (3.3V)

  // Voltage divider resistor values (adjust if you're using a divider)
  // const float R1 = 990.0;  // Resistor R1 in the voltage divider (in ohms)
  // const float R2 = 5000.0; // Resistor R2 in the voltage divider (in ohms)
  const float R1 = 10000.0;  // Resistor R1 in the voltage divider (in ohms)
  const float R2 = 100000.0; // Resistor R2 in the voltage divider (in ohms)
  // Calculate the divider ratio
  const float voltageDividerRatio = (R1 + R2) / R2;

  int adcValue = analogRead(batteryPin); // Read the ADC value
  // BATTERY_LOG("ADC value: ");
  // BATTERY_LOGLN(adcValue);
  float batteryVoltage = (adcValue / maxADCValue) * adcReferenceVoltage * voltageDividerRatio;
  batteryVoltage = batteryVoltage;
  Serial.print("battery Voltage: ");
  Serial.println(batteryVoltage);

  if (batteryVoltage < 2.70)
  {
    return 10;
  }
  else if (batteryVoltage < 2.77)
  {
    return 20;
  }
  else if (batteryVoltage < 2.87)
  {
    return 30;
  }
  else if (batteryVoltage < 3.09)
  {
    return 40;
  }
  else if (batteryVoltage < 3.15)
  {
    return 50;
  }
  else if (batteryVoltage < 3.21)
  {
    return 60;
  }
  else if (batteryVoltage < 3.25)
  {
    return 70;
  }
  else if (batteryVoltage < 3.30)
  {
    return 80;
  }
  else if (batteryVoltage < 3.38)
  {
    return 90;
  }

  else if (batteryVoltage <= 3.50)
  {
    return 100;
  }
  return 0;
}
