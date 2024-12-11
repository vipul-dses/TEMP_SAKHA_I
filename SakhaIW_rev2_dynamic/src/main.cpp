#include <Arduino.h>
#include "config.h"
#include "sensorLib.h"
#include "HX711.h"
#include "pinDefs.h"
#include "displayLib.h"
#include "espCom.h"
#include <esp_wifi.h>
#include <WiFi.h>
#include <esp_now.h>
#include <ledLib.h>
#include "SH1106Wire.h"
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP (3.6 * uS_TO_S_FACTOR)       //3.6e9 // 300 // 3.6e9
unsigned long previousMillis = 0;
const long period = 60000;
// #define uS_TO_S_FACTOR 1000000
// #define TIME_TO_SLEEP 60
float containerWeight;

RTC_DATA_ATTR int bootCount = 0;
TaskHandle_t Task1;
TaskHandle_t Task2;

void Task1code(void *pvParameters)
{

  bootCount = bootCount + 1;
  Serial.print("bootCount:");
  Serial.println(bootCount);
  // nowInit();
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  for (;;)
  {
    readSensor();
    // Serial.print("MAIN TASK WEIGHT : ");
    // Serial.println(TW);
    monitorDisplay();
    unsigned long currentMillis = millis(); // store the current time
    if (currentMillis - previousMillis >= period)
    {
      sleepSensor();
    //  delay(2000);
      turnOffScreen();
      esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
      Serial.println("Going to sleep now");
      esp_deep_sleep_start();
    }
    //  nowSend();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void Task2code(void *pvParameters)
{

  // GW = gasWeight;

  for (;;)
  {
  //          Serial.print("espNowFlag:");
  // Serial.println(espNowFlag);
  //        Serial.print("sensorFlag:");
  // Serial.println(sensorFlag);
    if (espNowFlag && sensorFlag)
    {

      nowSend();
      vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  // pinMode(batGPIO, OUTPUT);
  // digitalWrite(batGPIO, HIGH);
  Serial.begin(115200);
  pinInit();
  initializeLed();
  eData();
  nowInit();
  initializeDisplay();

  xTaskCreatePinnedToCore(Task1code, "Task1", 4000, NULL, 1, &Task1, 0);
  delay(10);
  xTaskCreatePinnedToCore(Task2code, "Task2", 4000, NULL, 1, &Task2, 1);
  delay(10);
}

void loop()
{
}
