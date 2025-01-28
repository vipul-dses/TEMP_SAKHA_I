#include "Arduino.h"
#include "pinDefs.h"
#include "displayLib.h"
#include "OLEDDisplay.h"
#include "SH1106Wire.h"
#include "Wire.h"
#include "SPI.h"
#include "sensorLib.h"
#include "ledLib.h"
#include "timeLib.h"
#include "wifiCom.h"

bool stopScroll = true;
extern float containerWeight;
float batteryPerc;
SH1106Wire display(0x3c, SDA, SCL);
bool screenSleep = false;
float TW = 0.0;
float CW = 0.0;
int GP;
float GW;
#define DEMO_DURATION 3000
typedef void (*Demo)(void);
int demoMode = 0;
int counter = 1;
bool runOutFlag=false;

void turnOffScreen()
{
  display.displayOff();
}
void screenOne()
{
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Regular_12);
  display.drawString(64, 0, "Your safety companion");
  display.setFont(Arimo_Regular_24);
  display.drawString(64, 15, "Sakha");
  display.setFont(Arimo_Regular_18);
  display.drawString(64, 42, "DSES Pvt Ltd");
  display.display();
}
void screenTwo()
{
  batteryPerc = getBatteryPercentage();
  display.clear();
  TW = weight;
  if (gasWeight < 0.0)
  {
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "TW: " + String(weight));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_24);
    display.drawString(64, 12, "- %");
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 38, "GW: - KG");
    display.display();
  }
  else if (gasWeight >= 0.0 && gasWeight <= 0.01)
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_12);
    display.drawString(64, 0, "Notification");
    display.setFont(Arimo_Regular_24);
    display.drawString(64, 12, String(0) + " %");
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 38, "Gas Runout");
    runOutFlag=true;
    display.display();
  }

  else if (gasWeight > 0.01 && gasWeight <=0.15)
  {
    display.clear();
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "TW: " + String(weight));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_24);
    display.drawString(64, 12, String(1) + " %");
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 38, "GW: " + String(gasWeight) + " Kg");
    display.display();
  }
    else if (gasWeight > 0.15 )
  {
    display.clear();
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "TW: " + String(weight));
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_24);
    display.drawString(64, 12, String(gasPercentage) + " %");
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 38, "GW: " + String(gasWeight) + " Kg");
    display.display();
  }

  // if (batteryPerc < 2.87)
  if (batteryPerc == 10 && !runOutFlag)
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_18);
    display.drawString(64, 15, "LOW BATTERY");
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_empty);
    display.display();
    runOutFlag=false;
    redColor();
  }

  // else if (batteryPerc < 3.09)
  else if (batteryPerc == 20 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("20%"));
    runOutFlag=false;
  }
  else if (batteryPerc == 30 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("30%"));
    runOutFlag=false;
  }

  else if (batteryPerc == 40 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("40%"));
    runOutFlag=false;
  }
  // else if (batteryPerc < 3.21)
  else if (batteryPerc == 50 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_medium);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("50%"));
    runOutFlag=false;
  }
  else if (batteryPerc == 60 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("60%"));
    runOutFlag=false;
  }
  else if (batteryPerc == 70 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("70%"));
    runOutFlag=false;
  }
  // else if (batteryPerc < 3.33)
  else if (batteryPerc == 80 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_high);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("80%"));
    runOutFlag=false;
  }
  else if (batteryPerc == 90 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_low);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(80, 0, "BP: " + String("90%"));
    runOutFlag=false;
  }
  // else if (batteryPerc <= 3.45)
  else if (batteryPerc == 100 && !runOutFlag)
  {
    // display.drawXbm(116, 0, battery_icon_width, battery_icon_height, battery_full);
    display.setFont(Arimo_Regular_12);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(70, 0, "BP: " + String("100%"));
    runOutFlag=false;
  }
  // display.drawXbm(116, 0, Bluetooth_width, Bluetooth_height, Bluetooth_Logo);
  display.display();
  //  delay(3000);
  //  display.displayOff();
}
void screenThree()
{

  display.clear();
  if (wC)
  {
    display.drawXbm(0, 0, WiFi_width, WiFi_height, WiFi_Logo);
  }

  display.drawXbm(116, 0, Bluetooth_width, Bluetooth_height, Bluetooth_Logo);

  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.setFont(Arimo_Regular_12);
  display.drawString(64, 0, cDay + ", " + cDate + " " + cMonth);
  display.setFont(Arimo_Regular_24);
  display.drawString(64, 25, cHour + ":" + cMinute + " " + sHour);
  // display.setFont(Arimo_Regular_18);
  // display.drawString(64, 42, "GC: " + String(GC) + " PPM");
  display.display();
}

Demo demos[] = {screenOne, screenTwo, screenThree};
int demoLength = sizeof(demos) / sizeof(Demo);
long timeSinceLastModeSwitch = 0;

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
  delay(500);
}
void monitorDisplay()
{
  if (stopScroll)
  {
    demos[demoMode]();
    if (millis() - timeSinceLastModeSwitch > DEMO_DURATION)
    {
      demoMode = (demoMode + 1) % demoLength;
      timeSinceLastModeSwitch = millis();
    }
    counter++;
    delay(10);
  }
}

void screenAck(String msg, float blecontainer)
{

  if (blecontainer > 0)
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_12);
    display.drawString(64, 0, "Notification");
    display.setFont(Arimo_Regular_12);
    display.drawString(64, 18, msg);
    display.setFont(Arimo_Regular_24);
    display.drawString(64, 30, String(blecontainer) + " KG");
    display.display();
    delay(1000);
    stopScroll = true;
  }
  else
  {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.setFont(Arimo_Regular_12);
    display.drawString(64, 0, "Notification");
    display.setFont(Arimo_Regular_12);
    display.drawString(64, 18, msg);
    display.display();
    delay(5000);
    stopScroll = true;
  }
}