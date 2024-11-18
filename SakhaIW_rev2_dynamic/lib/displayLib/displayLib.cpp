#include "Arduino.h"
#include "pinDefs.h"
#include "displayLib.h"
#include "OLEDDisplay.h"
#include "SH1106Wire.h"
#include "Wire.h"
#include "SPI.h"
#include "sensorLib.h"
#include "ledLib.h"

float batteryPerc;
SH1106Wire display(0x3c, SDA, SCL);
bool screenSleep = false;
float TW = 0.0;
float CW = 0.0;
int GP;
float GW;


void turnOffScreen()
{
  display.displayOff();
}

void initializeDisplay()
{
  display.init();
  display.flipScreenVertically();
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Regular_12);
  display.drawString(64, 0, "Your safety companion");
  display.setFont(Arimo_Regular_24);
  display.drawString(64, 15, "Sakha");
  display.setFont(Arimo_Regular_18);
  display.drawString(64, 42, "DSES Pvt Ltd");
  display.display();
  delay(2000);
}
void monitorDisplay()
{
  batteryPerc = getBatteryPercentage();
  // TW = readSensor();
  // voltage = 2.6;
  //  display.init();
  //  display.flipScreenVertically();
  display.clear();
  TW = weight;
  //  CW = incomingCW;
  // display.setTextAlignment(TEXT_ALIGN_CENTER);
  // display.setFont(Arimo_Regular_12);
  // display.drawString(64, 0, "Weight");
  // display.setFont(Arimo_Regular_24);
  // display.drawString(64, 15, String(TW));
  // display.setFont(Arimo_Regular_18);
  // display.drawString(64, 42, "KG");
  if (weight < 0.0)
  {
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "TW: " + String(weight));
    // display.setTextAlignment(TEXT_ALIGN_RIGHT);
    // display.drawString(128, 0, "CW: " + String(CW));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_24);
    display.drawString(64, 12, "- %");
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 38, "GW: - KG");
    display.display();
  }
  else if(weight >= 0.0 && weight <= 2.0 )
  {
    display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Notification");
        display.setFont(Arimo_Regular_24);
        display.drawString(64, 12, String(gasPercentage) + " %");
        display.setFont(Arimo_Regular_18);
        display.drawString(64, 38, "Gas Runout");
        display.display();
  }

  else if (weight > 2.0)
  {
    display.clear();
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0,0, "TW: " + String(weight));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_24);
    display.drawString(64,12,String(gasPercentage)+" %");
    display.setFont(Arimo_Regular_18);
    display.drawString(64,38, "GW: "+ String(gasWeight)+" Kg");
    display.display();
  }

  // if (batteryPerc < 2.87)
  if (batteryPerc == 10)
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 15, "LOW BATTERY");
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_empty);
    display.display();
    redColor();
  }
  // else if (batteryPerc < 3.09)
  else if (batteryPerc == 20)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("20%"));
  }
  else if (batteryPerc == 30)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("30%"));
  }

  else if (batteryPerc == 40)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("40%"));
  }
  // else if (batteryPerc < 3.21)
  else if (batteryPerc == 50)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_medium);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("50%"));
  }
  else if (batteryPerc == 60)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("60%"));
  }
  else if (batteryPerc == 70)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("70%"));
  }
  // else if (batteryPerc < 3.33)
  else if (batteryPerc == 80)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_high);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("80%"));
  }
  else if (batteryPerc == 90)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("90%"));
  }
  // else if (batteryPerc <= 3.45)
  else if (batteryPerc == 100)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_full);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(70, 0, "BP: " + String("100%"));
  }
  // display.drawXbm(116, 0, Bluetooth_width, Bluetooth_height, Bluetooth_Logo);
  display.display();
  //  delay(3000);
  //  display.displayOff();
}
