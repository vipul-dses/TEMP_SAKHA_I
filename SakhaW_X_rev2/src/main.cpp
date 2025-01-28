#include <Arduino.h>
#include "config.h"
#include "sensorLib.h"
#include "HX711.h"
#include <Adafruit_I2CDevice.h>
#include "timeLib.h"
#include "buzzerLib.h"
#include "ledLib.h"
#include "displayLib.h"
#include "bleCom.h"
#include "wifiCom.h"
#include "pinDefs.h"
#include "Preferences.h"
#include "flashMemory.h"
#include "otaLib.h"

Preferences mPreferences;
float containerWeight;
int normalMode = false;
#define uS_TO_S_FACTOR 1000000
 #define TIME_TO_SLEEP 3600000000
//#define TIME_TO_SLEEP 120000000 // 2 min
unsigned long previousMillis = 0;
const long periodMajor = 300000; // 5min    //First boot
const long periodMinor = 45000;  // 30sec  //Reboot device run time
const long periodNormal = 60000; // 1hour // Normal mode data log interval

int disBuzzer;
int enReminder;
int remDay;
int remHour;
int remMinute;
String remMessage;

RTC_DATA_ATTR int bootCount = 0;

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;
TaskHandle_t Task7;

void Task1code(void *pvParameters)
{
  bootCount = bootCount + 1;
  Serial.print("bootCount:");
  Serial.println(bootCount);
  if (bootCount == 1)
  {
    initializeBle();
  }
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  for (;;)
  {
    UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task1);
   // readSensor();
    monitorTime();
    if ((enReminder == 1) && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute)
    {
     // reminderOn=true;
      yellowColor();
      if (disBuzzer == 0)
      {
        buzzerBeepR();
      }
    }
    if (stopScroll)
    {
      monitorDisplay();
    }

    unsigned long currentMillis = millis();                             // store the current time
    if (normalMode && (currentMillis - previousMillis >= periodNormal)) // add one hour time
    {
      writeToSPIFFS();
      previousMillis = currentMillis;
    }
    if (bootCount == 1 && !normalMode)
    {
      if (currentMillis - previousMillis >= periodMajor)
      {
        writeToSPIFFS();
        sleepSensor();
        //  delay(2000);
        turnOffScreen();
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
        Serial.println("Going to sleep now");
        esp_deep_sleep_start();
      }
    }
    else if (bootCount >= 2 && !normalMode)
    {
      if (currentMillis - previousMillis >= periodMinor)
      {
        writeToSPIFFS();
        sleepSensor();
        turnOffScreen();
        esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
        Serial.println("Going to sleep now");
        esp_deep_sleep_start();
      }
    }
    // Serial.printf("Task11111: %u\n", stackWaterMark);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task3code(void *pvParameters)
{
  for (;;)
  {
    UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task3);
    // Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
    if (bNP)
    {
      mPreferences.begin("mD", false);
      mPreferences.putString("wN", wifiName);
      mPreferences.putString("wP", wifiPass);
      mPreferences.end();
      BLE_PRINTLN("Saved data in namePass: " + wifiName + " " + wifiPass);
      ESP.restart();
    }
    else if (bT)
    {
      cTime = blTime;
      updateTime();
      bT = false;
    }

    else if (bDB)
    {
      disBuzzer = blDisBuzzer;
      mPreferences.begin("mD", false);
      mPreferences.putInt("dB", disBuzzer);
      mPreferences.end();
      BLE_PRINTLN("Saved data in disBuzzer: " + String(disBuzzer));
      bDB = false;
    }
    else if (bER)
    {
      enReminder = blEnReminder;
      mPreferences.begin("mD", false);
      mPreferences.putInt("eR", enReminder);
      mPreferences.end();
      BLE_PRINTLN("Saved data in enReminder: " + String(enReminder));
      bER = false;
    }
    else if (bR)
    {
      remDay = blRemDay;
      remHour = blRemHour;
      remMinute = blRemMinute;
      remMessage = blRemMessage;
      mPreferences.begin("mD", false);
      mPreferences.putInt("rD", remDay);
      mPreferences.putInt("rH", remHour);
      mPreferences.putInt("rMi", remMinute);
      mPreferences.putString("rMe", remMessage);
      mPreferences.end();
      BLE_PRINTLN("Saved data in dHMM: " + String(remDay) + " " + String(remHour) + " " + String(remMinute) + " " + remMessage);
      bR = false;
    }
    else if (bRM)
    {
      regulatorMode = blRegulatorMode;
      mPreferences.begin("mD", false);
      mPreferences.putInt("rM", regulatorMode);
      mPreferences.end();
      BLE_PRINTLN("Saved data in regulatorMode: " + String(regulatorMode));
      bRM = false;
    }
    else if (bCW)
    {
      containerWeight = blContainerWeight;
      stopScroll = false;
      screenAck("Received CW:", blContainerWeight);
      mPreferences.begin("mD", false);
      mPreferences.putFloat("cW", containerWeight);
      mPreferences.end();
      BLE_PRINTLN("Saved data in containerWeight: " + String(containerWeight));
      bCW = false;
    }

    else if (bModeFlag)
    {

      normalMode = bMode;
      BLE_PRINTLN("Saved data in Mode: " + String(normalMode));
      mPreferences.begin("mD", false);
      mPreferences.putBool("MO", normalMode);
      mPreferences.end();
      bModeFlag = false;
    }
    else if (crDataflag)
    {
      CRToSPIFFS();
    }
    else if (blcrFlag)
    {
      SPIFFStoCR();
    }


    BWN = wifiName;
    BDB = disBuzzer;
    BER = enReminder;
    BRD = remDay;
    BRH = remHour;
    BRMi = remMinute;
    BRMe = remMessage;
    BTW = weight;
    BCW = containerWeight;
    BRMo = regulatorMode;
        if (bGraph)
    {
      delay(100);
      listDir(SPIFFS, "/", 0);
      bGraph = 0;
    }
    if (!bGraph && bootCount == 1)
    {
      monitorBle();
    }
    //  Serial.printf("Task333333: %u\n", stackWaterMark);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task4code(void *pvParameters)
{
  for (;;)
  {
    UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task4);

    ssid = wifiName.c_str();
    password = wifiPass.c_str();
    if (wT)
    {
      cTime = wifiTime;
      updateTime();
      wT = false;
    }
    else if (wDB)
    {
      disBuzzer = wifiDisBuzzer;
      mPreferences.begin("mD", false);
      mPreferences.putInt("dB", disBuzzer);
      mPreferences.end();
      SERIAL_PRINTLN("Saved data in disBuzzer: " + String(disBuzzer));
      wDB = false;
    }
    else if (wRM)
    {
      regulatorMode = wRegulatorMode;
      mPreferences.begin("mD", false);
      mPreferences.putInt("rM", regulatorMode);
      mPreferences.end();
      BLE_PRINTLN("Saved data in regulatorMode: " + String(regulatorMode));
      wRM = false;
    }

        else if (wER)
    {
      enReminder = wifiEnReminder;
      mPreferences.begin("mD", false);
      mPreferences.putInt("eR", enReminder);
      mPreferences.end();
      BLE_PRINTLN("Saved data in enReminder: " + String(enReminder));
      wER = false;
    }
    else if (wR)
    {
      remDay = wifiRemDay;
      remHour = wifiRemHour;
      remMinute = wifiRemMinute;
      remMessage = wifiRemMessage;
      mPreferences.begin("mD", false);
      mPreferences.putInt("rD", remDay);
      mPreferences.putInt("rH", remHour);
      mPreferences.putInt("rMi", remMinute);
      mPreferences.putString("rMe", remMessage);
      mPreferences.end();
      BLE_PRINTLN("Saved data in dHMM: " + String(remDay) + " " + String(remHour) + " " + String(remMinute) + " " + remMessage);
      wR = false;
    }
    else if (wCW)
    {
      containerWeight = wContainerWeight;
    stopScroll = false;
      screenAck("Received CW:", wContainerWeight);
      mPreferences.begin("mD", false);
      mPreferences.putFloat("cW", containerWeight);
      mPreferences.end();
      BLE_PRINTLN("Saved data in containerWeight: " + String(containerWeight));
      wCW = false;
    }
    else if (wModeFlag)
    {
      normalMode = wMode;
      mPreferences.begin("mD", false);
      mPreferences.putInt("MO", normalMode);
      mPreferences.end();
      wModeFlag = false;
    }

    if (wGraph)
    {
      delay(100);
      listDir(SPIFFS, "/", 0);
      wGraph = 0;
    }
    if (!wGraph)
    {
      monitorWiFi();
    }

    WWN = wifiName;
    WDB = disBuzzer;
    WRMo = regulatorMode;
    WTW = weight;
    WCW = containerWeight;
    WER = enReminder;
    // Serial.printf("Task444444: %u\n", stackWaterMark);

    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void Task7code(void *pvParameters)
{
  for (;;)
  {
    UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task7);
  //  monitorCom();
  gasWeight=readSensor();
  Serial.println(gasWeight);
    if (gasWeight < 0.0 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
       lightColor();
    }
    else if (gasWeight >= 0.0 && gasWeight <= 1.0 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      blueColor();
    }
    else if (gasWeight > 1.0 && gasWeight <= 2.0  && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      aquaColor();
    }
    else if (gasWeight > 2.0 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      greenColor();
    }
    // Serial.printf("Task7777777: %u\n", stackWaterMark);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void initializeData()
{
  mPreferences.begin("mD", false);
  wifiName = mPreferences.getString("wN", wifiName);
  wifiPass = mPreferences.getString("wP", wifiPass);
  SERIAL_PRINTLN("Saved data in namePass: " + wifiName + " " + wifiPass);
  disBuzzer = mPreferences.getInt("dB", disBuzzer);
  SERIAL_PRINTLN("Saved data in disBuzzer: " + String(disBuzzer));
  normalMode = mPreferences.getInt("MO", normalMode);
  SERIAL_PRINTLN("Saved data in normalMode: " + String(normalMode));
  regulatorMode = mPreferences.getInt("rM", regulatorMode);
  SERIAL_PRINTLN("Saved data in regulatorMode: " + String(regulatorMode));
  containerWeight = mPreferences.getFloat("cW", containerWeight);
  SERIAL_PRINTLN("Saved data in containerWeight: " + String(containerWeight));
  mPreferences.end();
}

void setup()
{
  Serial.begin(115200);
  pinInit();
  initializeData();
  // initializeCom();
  initializeTime();
  initializeLed();
  initializeBuzzer();
  initializeDisplay();
  bool isOTA = checkAndResumeOTA();
  if (isOTA)
  {
    return;
  }
  initSPIFFS();

  xTaskCreatePinnedToCore(Task1code, "Task1", 4000, NULL, 1, &Task1, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task3code, "Task3", 4000, NULL, 1, &Task3, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task4code, "Task4", 4000, NULL, 1, &Task4, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task7code, "Task7", 4000, NULL, 1, &Task7, 0);
  delay(10);
}

void loop()
{
}
