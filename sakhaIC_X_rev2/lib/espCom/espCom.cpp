#include "Arduino.h"
#include "config.h"
#include "espCom.h"
#include "WiFi.h"
#include "esp_now.h"
#include "Preferences.h"
#include "buzzerLib.h"
#include "bleCom.h"
#include "wifiCom.h"
#include "flashMemory.h"

Preferences ePreferences;
//uint8_t broadcastAddress[] = {0xEC, 0x64, 0xC9, 0x0A, 0x28, 0x9C};     //SAKHAW_REV3_RTC  
//uint8_t broadcastAddress[]= {0xEC, 0xC9, 0xFF, 0xCC, 0x64, 0x2C};    //SAKHAW_REV4_2    
uint8_t broadcastAddress[] = {0xEC, 0x64, 0xC9, 0x09, 0xF1, 0x18}; // SAKHAW_REV4_1    :for connecting White enclosure use this

float incomingWeight;
int incomingbatteryVoltage;
float incomingCW;

String success;
extern float containerWeight;
float totalWeight;
bool eD = false;
int eDCounter =0;

typedef struct struct_message
{
  float w;
  float cwnow;
  float batterynow;
  int regulatornow;

} struct_message;
struct_message receiveData;

struct_message outgoingReadings;

struct_message incomingReadings;

esp_now_peer_info_t peerInfo;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0)
  {
    success = "Delivery Success :)";
  }
  else
  {
    success = "Delivery Fail :)";
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  if (wifiDisBuzzer == 0 || blDisBuzzer == 0)
  {
    buzzerBeepAck();
  }
  buzzerBeepAck();
  incomingWeight = incomingReadings.w;
   incomingCW = incomingReadings.cwnow;
  incomingbatteryVoltage = incomingReadings.batterynow;
  NOW_PRINT("incomingWeight: ");
  NOW_PRINTLN(incomingWeight);
  NOW_PRINT("Received battery voltage: ");
  NOW_PRINTLN(incomingbatteryVoltage);
  totalWeight = incomingWeight;

    Serial.print("incomingCW: ");
    Serial.println(incomingCW);

  ePreferences.begin("mB", false);
  ePreferences.putInt("mB", incomingbatteryVoltage);
  ePreferences.begin("eD", false);
  ePreferences.putFloat("tW", totalWeight);
  ePreferences.end();
  eD = true;
  eDCounter=eDCounter+1;
  SERIAL_PRINTLN("Saved data in totalWeight: " + String(totalWeight));
  SERIAL_PRINTLN("Received data from Sakha (W):" + String(totalWeight));
  outgoingReadings.w = 7;
  // send container weight here
  outgoingReadings.cwnow = containerWeight;
  outgoingReadings.batterynow = 0;
  outgoingReadings.regulatornow = regulatorMode;
  // outgoingReadings.cwnow = 8;
  delay(100);
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }

  else
  {
    Serial.println("Error sending the data");
  }
  delay(500);
  writeToSPIFFS();
  // listDir(SPIFFS, "/", 0);
}

void initializeCom()
{
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK)
  {
    SERIAL_PRINTLN("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK)
  {
    Serial.println("Failed to add peer");
    return;
  }

  // esp_now_register_recv_cb(onDataRecv);
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void eData()
{
  ePreferences.begin("eD", false);
  totalWeight = ePreferences.getFloat("tW", totalWeight);
  SERIAL_PRINTLN("Saved data in totalWeight: " + String(totalWeight));
  incomingbatteryVoltage = ePreferences.getInt("mB", incomingbatteryVoltage);
  ePreferences.end();
}