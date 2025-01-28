#include "Arduino.h"
#include "config.h"
#include "wifiCom.h"
#include "WiFi.h"
#include "WebServer.h"
#include "WiFiUdp.h"
#include "ESPmDNS.h"
#include "ArduinoJson.h"
#include "buzzerLib.h"
#include "sensorLib.h"
#include "SPIFFS.h"
#include "FS.h"

const char *ssid;
const char *password;
extern int normalMode;

String WWN;
int WDB;
int WRMo;
int WER;
int WRD;
int WRH;
int WRMi;
String WRMe;
float WTW;
float WCW;
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
bool wWS = false;
bool wGraph = false;
bool wcrFlag = false;

WebServer server(80);
#define UDP_PORT 50023
WiFiUDP udp;
char incomingUDPPacket[255];
bool wModeFlag = false;
bool wMode = false;
bool wRM = false;
int wRegulatorMode;
float wContainerWeight;
bool wCW;

void WSPIFFStoCR()
{
  Serial.println("123456");
  // File file = SPIFFS.open("/hello.txt");
  File file = SPIFFS.open("/hello.txt", "r");
  if (!file)
  {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  String data1;
  while (file.available())
  {
    //   Serial.write(file.read());
    data1 += (char)file.read();
  }
  file.close();
  String data2 = "{\"Re\": 1, \"data\": [";
  data1.remove(data1.length() - 1);
  Serial.println(data1);
  String data3 = "]}";
  String data4 = data2 + data1 + data3;
  server.send(200, "text/json", data4);
  Serial.println("Wi-Fi Data sent: " + data4);
}

void wAppendFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- message appended");
  }
  else
  {
    Serial.println("- append failed");
  }
  file.close();
}

// void WCRToSPIFFS(String data)
// {
//   //  deleteFile(SPIFFS, "/hello.txt");
//   data += ',';
//   const char *dataStr = data.c_str();
//   wAppendFile(SPIFFS, "/hello.txt", dataStr);
//   //  readFile(SPIFFS, "/hello.txt");
// }
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
  jsonDocWT["BP"] = 100;//getBatteryPercentage(); // incomingbatteryVoltage;
  jsonDocWT["MO"] = normalMode;

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
    wContainerWeight = jsonDocWR["CW"]; // container_weight
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    wCW = true;
    jsonDocWR["Re"] = 1; // response: success
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

   // WIFI_PRINTLN("wifiEnReminder: " + String(wifiEnReminder));
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
    wER = true;
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
   // WIFI_PRINTLN("Sent: " + wData);

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
    //WIFI_PRINTLN("dHMM: " + String(wifiRemDay) + " " + String(wifiRemHour) + " " + String(wifiRemMinute) + " " + wifiRemMessage);
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
    jsonDocWT["Re"] = 1;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    return;
  }
  jsonDocWT["Re"] = 0;
  serializeJson(jsonDocWT, wData);
  server.send(400, "text/json", wData);
}

void handleMode()
{
  DynamicJsonDocument jsonDocWT(1024);
  String wData;
  if (server.hasArg("Va"))
  {
    String value = server.arg("Va");
    DynamicJsonDocument jsonDocWR(1024);
    deserializeJson(jsonDocWR, value);
    wModeFlag = true;
    wMode = jsonDocWR["MO"];
    SERIAL_PRINTLN("Mode : " + String(wMode));
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
    if (wifiDisBuzzer == 0)
    {
      buzzerBeepAck();
    }
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
        WiFi.setHostname("Sakha (W)");
        WiFi.begin(ssid, password);
        SERIAL_PRINTLN("Connecting to " + String(ssid) + ":" + String(password));
        vTaskDelay(30000 / portTICK_PERIOD_MS);
      }
      SERIAL_PRINT("Dynamic IP Address: ");
      SERIAL_PRINTLN(WiFi.localIP());
      if (!MDNS.begin("sakha-w"))
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
      server.on("/wgraph", handlegraph);
      server.on("/wMode", handleMode);
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
}
