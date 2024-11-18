/*Sakha-C Beta update
Uploded into new Sakha-I units
Sakha-W ESP8266 variant
Last update on:6/7/2024*/
#include "Arduino.h"
#include "config.h"
#include "sensorLib.h"
#include "timeLib.h"
#include "buzzerLib.h"
#include "ledLib.h"
#include "displayLib.h"
#include "bleCom.h"
#include "wifiCom.h"
#include "espCom.h"
#include "Preferences.h"
#include "flashMemory.h"
Preferences mPreferences;
TaskHandle_t Task1;
TaskHandle_t Task2;
TaskHandle_t Task3;
TaskHandle_t Task4;
TaskHandle_t Task5;
TaskHandle_t Task6;
TaskHandle_t Task7;
TaskHandle_t Task8;

int disBuzzer;
int enReminder;
int remDay;
int remHour;
int remMinute;
String remMessage;
 String totalWeightstr;
 int indexDecimal;
void Task1code(void *pvParameters)
{
  for (;;)
  {
    monitorTime();
    if (gasConc < 2 && enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute)
    {
      yellowColor();
      if (disBuzzer == 0)
      {
        buzzerBeepR();
      }
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task2code(void *pvParameters)
{
  for (;;)
  {
    iW = wC;
    iR = enReminder;
    GC = gasConc;
    HR = cHour;
    ME = cMinute;
    DY = cDay;
    DE = cDate;
    MH = cMonth;
    RM = remMessage;
    iE = eD;
    TW = totalWeight;
    CW = containerWeight;
    GP = gasPercentage;
    GW = gasWeight;
    monitorDisplay();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void Task3code(void *pvParameters)
{
  for (;;)
  {
    if (bNP)
    {
      mPreferences.begin("mD", false);
      mPreferences.putString("wN", wifiName);
      mPreferences.putString("wP", wifiPass);
      mPreferences.end();
      BLE_PRINTLN("Saved data in namePass: " + wifiName + " " + wifiPass);
      bleAck("Connecting To " + wifiName, 0);
      ESP.restart();
    }
    else if (bT)
    {
      cTime = blTime;
      updateTime();
      bleAck("Time Updated", 0);
      bT = false;
    }
    else if (bDB)
    {
      disBuzzer = blDisBuzzer;
      if (disBuzzer)
      {
        bleAck("Buzzer turned Off", 0);
      }

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
      stopScroll = false;
      if (regulatorMode)
      {
        bleAck("Regulator mode is ON", 0);
      }
      else
      {
        bleAck("Regulator mode is OFF", 0);
      }
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
      bleAck("Received CW:", blContainerWeight);
      mPreferences.begin("mD", false);
      mPreferences.putFloat("cW", containerWeight);
      mPreferences.end();
      BLE_PRINTLN("Saved data in containerWeight: " + String(containerWeight));
      bCW = false;
    }
    BGC = gasConc;
    BWN = wifiName;
    BDB = disBuzzer;
    BER = enReminder;
    BRD = remDay;
    BRH = remHour;
    BRMi = remMinute;
    BRMe = remMessage;
      totalWeightstr=String(totalWeight);
    indexDecimal=  totalWeightstr.indexOf('.');
  //         Serial.print("index is : ");
  // Serial.println(indexDecimal);
  totalWeightstr=totalWeightstr.substring(0,indexDecimal+2);
    ////Serial.print("before conversion: ");
  //Serial.println(totalWeight);
    //Serial.print("after conversion: ");
  //Serial.println(totalWeightstr);
    BTW = totalWeightstr;
    BCW = containerWeight;
    BRMo = regulatorMode;
    if (bGraph)
    {
      delay(100);
      listDir(SPIFFS, "/", 0);
      bGraph = 0;
    }
    if (!bGraph)
    {
      monitorBle();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task4code(void *pvParameters)
{
  for (;;)
  {
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
    WRMo = regulatorMode;
    WTW = totalWeight;
    WCW = containerWeight;
    WRH = remHour;
    WRMi = remMinute;
    WRMe = remMessage;
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
      bleAck("Sensor Warming up", 0);
      iS = false;
      for (int i = 0; i < 300; i++)
      {
        unsigned long digitalReading = analogRead(MQ6);
        GAS_PRINTLN("Digital Reading: " + String(digitalReading));
        gasConc = 0;
        GAS_PRINTLN("Gas Concentration: " + String(gasConc) + " PPM");
        if (!(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
        {
          greenColor();
          //   buzzerOff();
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
        GAS_PRINTLN("Digital Reading: " + String(digitalReading));
        gasConc = 0;
        GAS_PRINTLN("Gas Concentration: " + String(gasConc) + " PPM");
        if (!(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
        {
          greenColor();
          //  buzzerOff();
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
        // buzzerOff();
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

void Task6code(void *pvParameters)
{
  for (;;)
  {
    if (!eD && gasConc < 2 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      iE = false;
      whiteColor();
    }
    else if (eD)
    {
      iE = true;
      vTaskDelay(3300000 / portTICK_PERIOD_MS);
      eD = false;
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task7code(void *pvParameters)
{
  for (;;)
  {
    monitorCom();
    if (eD && gasWeight < 0.0 && gasConc < 2 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      // lightColor();
    }
    else if (eD && gasWeight >= 0.0 && gasWeight <= 1.0 && gasConc < 2 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      blueColor();
    }
    else if (eD && gasWeight > 1.0 && gasWeight <= 2.0 && gasConc < 2 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      aquaColor();
    }
    else if (eD && gasWeight > 2.0 && gasConc < 2 && !bWS && !wWS && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute))
    {
      greenColor();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void Task8code(void *pvParameters)
{
  for (;;)
  {
    if (eD && gasWeight >= 0.0 && gasWeight <= 1.0 && gasConc < 2 && !(enReminder == 1 && (remDay == rDay || remDay == 0) && remHour == rHour && remMinute == rMinute) && sHour == "PM")
    {
      if (disBuzzer == 0)
      {
        buzzerRunout();
      }
    }
    vTaskDelay(3600000 / portTICK_PERIOD_MS);
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
  regulatorMode = mPreferences.getInt("rM", regulatorMode);
  SERIAL_PRINTLN("Saved data in regulatorMode: " + String(regulatorMode));
  containerWeight = mPreferences.getFloat("cW", containerWeight);
  SERIAL_PRINTLN("Saved data in containerWeight: " + String(containerWeight));
  mPreferences.end();

}

void setup()
{
  Serial.begin(115200);
  initializeData();
  eData();
  initializeCom();
  initializeTime();
  initializeLed();
  initializeBuzzer();
  initializeDisplay();
  initializeBle();
  initSPIFFS();
  delay(10);
  xTaskCreatePinnedToCore(Task6code, "Task6", 2000, NULL, 1, &Task6, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task7code, "Task7", 2000, NULL, 1, &Task7, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task8code, "Task8", 2000, NULL, 1, &Task8, 0);
  delay(10);
  // Task detects the level of gas concentration,
  // determines whether the reminder is set, and sounds a buzzer if the set time and the actual time match.
  xTaskCreatePinnedToCore(Task1code, "Task1", 4000, NULL, 1, &Task1, 1);
  delay(10);
  // Task2 display various content on the display
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
