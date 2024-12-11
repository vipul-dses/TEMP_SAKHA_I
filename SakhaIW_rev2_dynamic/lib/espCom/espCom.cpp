#include "espCom.h"
#include <esp_wifi.h>
#include <WiFi.h>
#include <esp_now.h>
#include "ledLib.h"
#include "sensorLib.h"
#include "Preferences.h"
#include "displayLib.h"

Preferences mPreferences;


// unit 50: 08:f9:e0:a3:a9:c0    //home unit
//  unit 45:  08:f9:e0:af:67:44
// unit 48    08:d1:f9:6c:73:44
//  uint8_t broadcastAddress[] = {0xEC, 0x64, 0xC9, 0x0A, 0x28, 0x9C};    //SAKHAIW address
//uint8_t broadcastAddress[] = {0x08, 0xD1, 0xF9, 0x6C, 0x73, 0x44}; // SAKHAIC address
uint8_t broadcastAddress[] = {0x08, 0xF9, 0xE0, 0xA3, 0xA9, 0xC0};
float myweight;
int myChannel = 0;
int failCounter = 0;
int myID;


int mutipleFailCounter = 0;
bool enow = false;
bool espNowFlag =false;

// Structure example to send data

float incomingWeight;
float incomingCW;
float incomingbatteryVoltage;
String success;

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
  printWiFiChannel();
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status != ESP_NOW_SEND_SUCCESS)
  {
    failCounter++; // Increment fail counter on delivery failure

    if (failCounter >= 10)
    {
      myChannel = myChannel + 1;

      // whiteColor();
      yellowColor();
      esp_wifi_set_channel(myChannel, WIFI_SECOND_CHAN_NONE);

      if (myChannel > 15)
      {
        myChannel = 0; // Reset channel if it exceeds the maximum value
        mutipleFailCounter = mutipleFailCounter + 1;
        Serial.print("\r\ntried entire channel for \t");
        Serial.println(mutipleFailCounter);
      }
     
      failCounter = 0; // Reset the failure counter after incrementing the channel
    }
  }
  else
  { 
    // Packet sent successfully
    greenColor();
     espNowFlag=false;
    failCounter = 0; // Reset the failure counter on success
    mPreferences.begin("mD", false);
    Serial.println("Saved data in channel: " + String(myChannel));
    myChannel = mPreferences.putInt("wC", myChannel);
delay(100);
    // delay(500);
    //  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    //  Serial.println("Going to sleep now");
    //  esp_deep_sleep_start();
  }
  if (mutipleFailCounter > 2)
  {
    // esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // Serial.println("Multiple attempts failed");
    // esp_deep_sleep_start();
  }
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));

  // incomingWeight = incomingReadings.w;
  incomingCW = incomingReadings.cwnow;
  incomingbatteryVoltage = incomingReadings.cwnow;
  regulatorMode = incomingReadings.regulatornow;
  // Serial.print("incomingWeight: ");
  // Serial.println(incomingWeight);
  Serial.print("incomingRMo: ");
  Serial.println(regulatorMode);
  Serial.print("incomingCW: ");
  Serial.println(incomingCW);
  mPreferences.begin("mC", false);
  mPreferences.putInt("rM", regulatorMode);
  mPreferences.putFloat("CWnow", incomingCW);
  mPreferences.end();
  enow = true;
      esp_now_deinit();
     WiFi.mode(WIFI_OFF);


}

int printWiFiChannel()
{
  int wifiChannel = WiFi.channel();
  Serial.print("Current Wi-Fi Channel: ");
  Serial.println(wifiChannel);
  return wifiChannel;
}

void nowSend()
{
  // myweight = readSensor();
  // myData.b = 1;
  // myData.w = 20;    TW = weight;

  //  myData.d = false;
  Serial.print("ESP-NOW SENT WEIGHT : ");
  Serial.println(weight);
  //   Serial.print("ESP-NOW SENT Battery % : ");
  // Serial.println(getBatteryPercentage());
  outgoingReadings.w = weight; // readSensor();
 // outgoingReadings.cwnow = random(1, 30);
  outgoingReadings.batterynow = getBatteryPercentage();
  outgoingReadings.regulatornow = 1;
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&outgoingReadings, sizeof(outgoingReadings));
 // delay(500);
  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
}

void nowInit()
{
  WiFi.mode(WIFI_STA);
 
  esp_wifi_set_channel(myChannel, WIFI_SECOND_CHAN_NONE);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  printWiFiChannel();
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
espNowFlag=true;
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

  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  //delay(3000);
}


void eData()
{
  mPreferences.begin("mD", false);
  myChannel = mPreferences.getInt("wC", myChannel);
  Serial.println("last stored channel was: " + String(myChannel));
  mPreferences.begin("mC", false);
  incomingCW = mPreferences.getFloat("CWnow", incomingCW);
  regulatorMode = mPreferences.getInt("rM", regulatorMode);
  Serial.println("last stored container weight was: " + String(incomingCW));
  Serial.println("last stored RM was: " + String(regulatorMode));
  mPreferences.end();
}