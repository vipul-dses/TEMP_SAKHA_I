#include "Arduino.h"
#include "config.h"
#include "wifiCom.h"
#include "WiFi.h"
#include "WebServer.h"
#include "WiFiUdp.h"
#include "ESPmDNS.h"
#include "ArduinoJson.h"
#include "buzzerLib.h"
#include "espCom.h"


const char *ssid;
const char *password;

int WGC;
String WWN;
int WDB;
int WER;
int WRD;
int WRMo;
float WTW;
float WCW;
int WRH;
int WRMi;
String WRMe;
bool wC = false;
String wifiTime;
bool wT = false;
int wifiDisBuzzer;
bool wDB = false;
int wifiEnReminder;
bool wER = false;
int wifiRemDay;
int wifiRemHour;
int wifiRemMinute;
String wifiRemMessage;
bool wR = false;
int wifilwarmupSensor;
bool wWS = false;
bool wGraph = false;
WebServer server(80);
#define UDP_PORT 50023
WiFiUDP udp;
char incomingUDPPacket[255];

void handleRoot()
{
  DynamicJsonDocument jsonDocWT(1024);
  jsonDocWT["Ma"] = WiFi.macAddress();
  String wData;
  serializeJson(jsonDocWT, wData);
  server.send(200, "text/json", wData);
  SERIAL_PRINTLN(WiFi.macAddress());
}

void handleData()
{
  DynamicJsonDocument jsonDocWT(1024);
  jsonDocWT["GC"] = WGC;
  jsonDocWT["WN"] = WWN;
  jsonDocWT["DB"] = WDB;
  jsonDocWT["ER"] = WER;
  jsonDocWT["RD"] = WRD;
  jsonDocWT["RH"] = WRH;
  jsonDocWT["RMi"] = WRMi;
  jsonDocWT["RMe"] = WRMe;
  jsonDocWT["TW"] = WTW;
  jsonDocWT["CW"] = WCW;
  jsonDocWT["RMo"] = WRMo;
  jsonDocWT["BP"] = incomingbatteryVoltage;

  String wData;
  serializeJson(jsonDocWT, wData);
  server.send(200, "text/json", wData);
  SERIAL_PRINTLN("WiFi Data Sent: " + wData);
}

void handleTime()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wifiTime = jsonDocWR["Ti"].as<String>();
    SERIAL_PRINTLN("wifiTime: " + wifiTime);
    wT = true;
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleBuzzer()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wifiDisBuzzer = jsonDocWR["DB"];
    SERIAL_PRINTLN("wifiDisBuzzer: " + String(wifiDisBuzzer));
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    wDB = true;
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleRegulator()
{
  // JsonDocument jsonDocToBeSent;
  DynamicJsonDocument jsonDocWT(1024);
  String dataToBeSent;
  if (server.hasArg("Va"))
  {                                  // value
    String value = server.arg("Va"); // value
    Serial.println("Received: " + value);
    // JsonDocument jsonDoc;
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    regulatorMode = jsonDocWR["RMo"]; // regulator_mode
    jsonDocWR["Re"] = 1;              // response: success
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    serializeJson(jsonDocWR, dataToBeSent);
    Serial.println("Sent: " + dataToBeSent);
    server.send(200, "text/json", dataToBeSent);
    return;
  }
  jsonDocWT["Re"] = 0; // response: fail
  serializeJson(jsonDocWT, dataToBeSent);
  Serial.println("Sent: " + dataToBeSent);
  server.send(400, "text/json", dataToBeSent);
}

void handleContainer()
{
  // JsonDocument jsonDocWR;
  DynamicJsonDocument jsonDocWT(1024);

  String dataToBeSent;
  if (server.hasArg("Va"))
  {                                  // value
    String value = server.arg("Va"); // value
    Serial.println("Received: " + value);
    // JsonDocument jsonDoc;
    DynamicJsonDocument jsonDocWR(1024);

    deserializeJson(jsonDocWR, value);

    deserializeJson(jsonDocWR, value);
    containerWeight = jsonDocWR["CW"]; // container_weight
    jsonDocWR["Re"] = 1;               // response: success
    serializeJson(jsonDocWR, dataToBeSent);
    Serial.println("Sent: " + dataToBeSent);
    server.send(200, "text/json", dataToBeSent);
    return;
  }
  jsonDocWT["Re"] = 0; // response: fail
  serializeJson(jsonDocWT, dataToBeSent);
  Serial.println("Sent: " + dataToBeSent);
  server.send(400, "text/json", dataToBeSent);
}

void handleScreen()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wifiEnReminder = jsonDocWR["ER"];
    SERIAL_PRINTLN("wifiEnReminder: " + String(wifiEnReminder));
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    wER = true;
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleReminder()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wifiRemDay = jsonDocWR["RD"];
    wifiRemHour = jsonDocWR["RH"];
    wifiRemMinute = jsonDocWR["RMi"];
    wifiRemMessage = jsonDocWR["RMe"].as<String>();
    SERIAL_PRINTLN("dHMM: " + String(wifiRemDay) + " " + String(wifiRemHour) + " " + String(wifiRemMinute) + " " + wifiRemMessage);
    wR = true;
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}
void handlegraph()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wGraph = jsonDocWR["SF"];
    SERIAL_PRINTLN("Permission for graph: " + String(wGraph));
        if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
        jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
  }

void handleSensor()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wifilwarmupSensor = jsonDocWR["DB"];
    SERIAL_PRINTLN("wifilwarmupSensor: " + String(wifilwarmupSensor));
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    if (!wWS)
    {
      wWS = true;
      SERIAL_PRINTLN("wifilwarmupSensor Start");
    }
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void checkForUDPRequest()
{
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    int len = udp.read(incomingUDPPacket, 255);
    if (len > 0)
    {
      incomingUDPPacket[len] = '\0';
    }
    Serial.printf("UDP packet contents: %s\n", incomingUDPPacket);
    String idealIPReq = "SakhaLocalIPReq";
    if (idealIPReq.equals(incomingUDPPacket))
    {
      IPAddress broadcastIP = WiFi.localIP();
      broadcastIP[3] = 255;
      udp.beginPacket(broadcastIP, UDP_PORT);
      udp.print(WiFi.localIP());
      udp.endPacket();
    }
  }
}

void monitorWiFi()
{
  //     SERIAL_PRINTLN("Entered in WIFI task!");

  if (ssid != nullptr && ssid[0] != '\0')
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      SERIAL_PRINTLN("Inside status!");
      while (WiFi.status() != WL_CONNECTED)
      {
        SERIAL_PRINTLN("Setting up network!");
        wC = false;
        WiFi.setHostname("Sakha (I)");
        WiFi.begin(ssid, password);
        SERIAL_PRINTLN("Connecting to " + String(ssid) + ":" + String(password));
        vTaskDelay(30000 / portTICK_PERIOD_MS);
      }
      SERIAL_PRINT("Dynamic IP Address: ");
      SERIAL_PRINTLN(WiFi.localIP());
      if (!MDNS.begin("sakha-i"))
      {
        SERIAL_PRINTLN("Error setting up MDNS responder!");
      }
      else
      {
        SERIAL_PRINTLN("mDNS responder started");
      }
      MDNS.addService("dses", "tcp", 80);
      udp.begin(UDP_PORT);
      server.on("/", handleRoot);
      server.on("/data", handleData);
      server.on("/time", handleTime);
      server.on("/buzzer", handleBuzzer);
      server.on("/screen", handleScreen);
      server.on("/reminder", handleReminder);
      server.on("/regulator", handleRegulator);
      server.on("/container", handleContainer);
      server.on("/sensor", handleSensor);
      server.on("/wgraph", handlegraph);
      server.begin();
      SERIAL_PRINTLN("HTTP server Started");
    }
    else
    {
      wC = true;
      server.handleClient();
      checkForUDPRequest();
    }
  }
}
