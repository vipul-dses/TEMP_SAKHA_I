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
#include "SPIFFS.h"
#include "FS.h"

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
bool wcrFlag = false;
bool wRM = false;
int wRegulatorMode;
float wContainerWeight;
bool wCW = false;
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
  // delay(100);
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
  server.stop(); // Stops the server
  return;
}

void handleTime()
{
  // delay(100);

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
    //     if (serializeJson(jsonDocWT, wData) == 0) {
    //     SERIAL_PRINTLN("Failed to serialize JSON");
    //     server.send(500, "text/json", "{\"error\":\"Failed to generate JSON\"}");
    //     return;
    // }
    server.send(200, "text/json", wData);
    server.stop(); // Stops the server
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleBuzzer()
{
  //   delay(100);

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
    server.stop(); // Stops the server
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleRegulator()
{
  //   delay(100);

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
    wRegulatorMode = jsonDocWR["RMo"]; // regulator_mode
    wRM = true;
    jsonDocWR["Re"] = 1; // response: success
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    serializeJson(jsonDocWR, dataToBeSent);
    Serial.println("Sent: " + dataToBeSent);
    server.send(200, "text/json", dataToBeSent);
    server.stop(); // Stops the server
    return;
  }
  jsonDocWT["Re"] = 0; // response: fail
  serializeJson(jsonDocWT, dataToBeSent);
  Serial.println("Sent: " + dataToBeSent);
  server.send(400, "text/json", dataToBeSent);
}

void handleContainer()
{
  // delay(100);

  // JsonDocument jsonDocWR;
  DynamicJsonDocument jsonDocWT(1024);

  String dataToBeSent;
  if (server.hasArg("Va"))
  {                                  // value
    String value = server.arg("Va"); // value
    Serial.println("Received: " + value);
    // JsonDocument jsonDoc;
    DynamicJsonDocument jsonDocWR(1024);

    //   deserializeJson(jsonDocWR, value);

    deserializeJson(jsonDocWR, value);
    wContainerWeight = jsonDocWR["CW"]; // container_weight
    wCW = true;
    jsonDocWR["Re"] = 1;
    jsonDocWR["Re"] = 1; // response: success
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    } // response: success
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
  //  delay(100);

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
    Serial.println("Sent: " + wData);

    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleReminder()
{
  //  delay(100);

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
    wGraph = true; // jsonDocWR["SF"];
    SERIAL_PRINTLN("Permission for graph: " + String(wGraph));
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    server.stop(); // Stops the server
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}


void WSPIFFStoCR()
{
  Serial.println("CR WIFI function");
  String data2 = "{\"Re\": 1, \"data\": [";
  String data1;
  for (int i = 1000; i <= 1050; i++)
  {
    String filename = "/" + String(i) + ".txt"; // Generate filename
    File file = SPIFFS.open(filename, "r");
    if (!file)
    {
      Serial.println("- failed to open file: " + filename);
      continue; // Skip to the next file
    }
   // Serial.println("- reading from file: " + filename);
    while (file.available())
    {
      data1 += (char)file.read();
    }
    int data1Len = data1.length();
    //Serial.println("File length: " + String(data1Len));
    if (data1Len == 0)
    {
      i = 1051;
    }
    
    file.close();
  }
  Serial.println("Combined Data:");
  Serial.println(data1);
  data1.remove(data1.length() - 1);
  String data3 = "]}";

  String data4 = data2 + data1 + data3;
  Serial.println("CR Data sent: " + data4);
  
  if (wcrFlag)
  {
    server.send(200, "text/json", data4);
    Serial.println("Wi-Fi Data sent print: " + data4);
    data4 = "";
    data1 = ""; // Clear data1 for the next file
   
    delay(100);
  }
  
}



void handlecylinder()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    Serial.println(value);
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    int type = jsonDocWR["Ty"].as<int>();
    if (type == 10)
    {
      // esp to app req
      wcrFlag = true;
      WSPIFFStoCR();
    }
    else if (type == 9)
    {
      // app to esp req
      WCRToSPIFFS(value);
      jsonDocWT["Re"] = 1;
      serializeJson(jsonDocWT, wData);
      server.send(200, "text/json", wData);
    }
    else
    {
      jsonDocWT["Re"] = 0;
      serializeJson(jsonDocWT, wData);
      server.send(400, "text/json", wData);
    }
    // if (wifiDisBuzzer == 0)
    // {
    //   buzzerBeepAck();
    // }
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleSensor()
{
  //   delay(100);

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
  // Serial.println("Free Heap: " + String(ESP.getFreeHeap()));

  if (ssid != nullptr && ssid[0] != '\0')
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      while (WiFi.status() != WL_CONNECTED)
      {
        SERIAL_PRINTLN("Setting up network!");
        wC = false;
        WiFi.setHostname("Sakha (I)");
        WiFi.begin(ssid, password);
        SERIAL_PRINTLN("Connecting to " + String(ssid) + ":" + String(password));
        vTaskDelay(3000 / portTICK_PERIOD_MS);
      }
      if (WiFi.status() == WL_CONNECTED)
      {
        SERIAL_PRINTLN("WiFi reconnected, restarting server...");
        server.begin();
      }
      else
      {
        SERIAL_PRINTLN("WiFi connection still failed after reconnect attempts.");
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
      server.on("/cylinders", handlecylinder);
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
  server.begin();
}
