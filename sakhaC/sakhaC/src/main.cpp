#include "Arduino.h"
#include "config.h"
#include "sensorLib.h"
#include "timeLib.h"
#include "buzzerLib.h"
#include "ledLib.h"
#include "displayLib.h"
#include "bleCom.h"
#include "wifiCom.h"
#include "Preferences.h"
#include "otaLib.h"
#include "firebaseLib.h"

Preferences mPreferences;

TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;
TaskHandle_t Task5;

int disBuzzer;
int enReminder;
int remDay;
int remHour;
int remMinute;
String remMessage;
/****************************************************************************************************************************************/
// Task1code Monitors reminder condition
void Task1code(void *pvParameters)
{
  for (;;)
  {
    //UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task1);
    monitorTime();
    if (gasConc < 2 && enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute)
    {
      yellowColor();
      if (disBuzzer == 0)
      {
        buzzerBeepR();
      }
    }
    //Serial.printf("update2 %u\n", stackWaterMark);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/****************************************************************************************************************************************/
// Task2code handles display related paramters
void Task2code(void *pvParameters)
{
  for (;;)
  {
    //UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task2);
    iW = wC;
    iR = enReminder;
    GC = gasConc;
    HR = cHour;
    ME = cMinute;
    DY = cDay;
    DE = cDate;
    MH = cMonth;
    RM = remMessage;
    monitorDisplay();
    //Serial.printf("Task2222: %u\n", stackWaterMark);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/****************************************************************************************************************************************/
// Task3code handels Bluetooth communication
void Task3code(void *pvParameters)
{
  for (;;)
  {
    //UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task3);
    if (bNP)
    {
      mPreferences.begin("mD", false);
      mPreferences.putString("wN", wifiName);
      mPreferences.putString("wP", wifiPass);
      mPreferences.end();
      SERIAL_PRINTLN("Saved data in namePass: " + wifiName + " " + wifiPass);
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
      SERIAL_PRINTLN("Saved data in disBuzzer: " + String(disBuzzer));
      bDB = false;
    }
    else if (bER)
    {
      enReminder = blEnReminder;
      mPreferences.begin("mD", false);
      mPreferences.putInt("eR", enReminder);
      mPreferences.end();
      SERIAL_PRINTLN("Saved data in enReminder: " + String(enReminder));
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
      SERIAL_PRINTLN("Saved data in dHMM: " + String(remDay) + " " + String(remHour) + " " + String(remMinute) + " " + remMessage);
      bR = false;
    }
    BGC = gasConc;
    BWN = wifiName;
    BDB = disBuzzer;
    BER = enReminder;
    BRD = remDay;
    BRH = remHour;
    BRMi = remMinute;
    BRMe = remMessage;
    monitorBle();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task4code(void *pvParameters)
{
  for (;;)
  {
    //UBaseType_t stackWaterMark = uxTaskGetStackHighWaterMark(Task4);
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
    else if (wER)
    {
      enReminder = wifiEnReminder;
      mPreferences.begin("mD", false);
      mPreferences.putInt("eR", enReminder);
      mPreferences.end();
      SERIAL_PRINTLN("Saved data in enReminder: " + String(enReminder));
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
      SERIAL_PRINTLN("Saved data in dHMM: " + String(remDay) + " " + String(remHour) + " " + String(remMinute) + " " + remMessage);
      wR = false;
    }
    WGC = gasConc;
    WWN = wifiName;
    WDB = disBuzzer;
    WER = enReminder;
    WRD = remDay;
    WRH = remHour;
    WRMi = remMinute;
    WRMe = remMessage;
    monitorWiFi();
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void Task5code(void *pvParameters)
{
  for (;;)
  {
    if (bWS)
    {
      wWS = true;
      iS = false;
      for (int i = 0; i < 300; i++)
      {
        unsigned long digitalReading = analogRead(MQ6);
        SERIAL_PRINTLN("Digital Reading: " + String(digitalReading));
        gasConc = 0;
        SERIAL_PRINTLN("Gas Concentration: " + String(gasConc) + " PPM");
        if (!(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
        {
          greenColor();
          buzzerOff();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
      iS = true;
      wWS = false;
      bWS = false;
    }
    else if (wWS)
    {
      bWS = true;
      iS = false;
      for (int i = 0; i < 180; i++)
      {
        unsigned long digitalReading = analogRead(MQ6);
        SERIAL_PRINTLN("Digital Reading: " + String(digitalReading));
        gasConc = 0;
        SERIAL_PRINTLN("Gas Concentration: " + String(gasConc) + " PPM");
        if (!(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
        {
          greenColor();
          buzzerOff();
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
      }
      iS = true;
      bWS = false;
      wWS = false;
    }
    else
    {
      monitorSensor();
      if (gasConc < 2 && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
      {
        greenColor();
        buzzerOff();
      }
      else if (gasConc >= 2 && gasConc < 5)
      {
        orangeColor();
        if (disBuzzer == 0)
        {
          buzzerBeepL();
        }
      }
      else if (gasConc >= 5)
      {
        redColor();
        if (disBuzzer == 0)
        {
          buzzerOn();
        }
      }
    }
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
  enReminder = mPreferences.getInt("eR", enReminder);
  SERIAL_PRINTLN("Saved data in enReminder: " + String(enReminder));
  remDay = mPreferences.getInt("rD", remDay);
  remHour = mPreferences.getInt("rH", remHour);
  remMinute = mPreferences.getInt("rMi", remMinute);
  remMessage = mPreferences.getString("rMe", remMessage);
  SERIAL_PRINTLN("Saved data in dHMM: " + String(remDay) + " " + String(remHour) + " " + String(remMinute) + " " + remMessage);
  mPreferences.end();
}

void setup()
{
  Serial.begin(115200);
  initializeData();
  initializeLed();
  initializeTime();
  initializeBuzzer();
  initializeDisplay();
  initializeBle();
  delay(10);
  xTaskCreatePinnedToCore(Task1code, "Task1", 4000, NULL, 1, &Task1, 1);
  delay(10);
  xTaskCreatePinnedToCore(Task2code, "Task2", 4000, NULL, 1, &Task2, 1);
  delay(10);
  xTaskCreatePinnedToCore(Task3code, "Task3", 4000, NULL, 1, &Task3, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task4code, "Task4", 4000, NULL, 1, &Task4, 0);
  delay(10);
  initializeSensor();
  iS = true;
  delay(10);
  xTaskCreatePinnedToCore(Task5code, "Task5", 4000, NULL, 1, &Task5, 1);
  delay(10);
}

void loop() {}