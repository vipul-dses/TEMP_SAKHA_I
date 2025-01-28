#include "Arduino.h"
#include "config.h"
#include "wifiCom.h"
#include "WiFi.h"
#include "WebServer.h"
#include "WiFiUdp.h"
#include "ESPmDNS.h"
#include "ArduinoJson.h"
#include "buzzerLib.h"

const char *ssid;
const char *password;

int WGC;
String WWN;
int WDB;
int WER;
int WRD;
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

WebServer server(80);
#define UDP_PORT 50023
WiFiUDP udp;
char incomingUDPPacket[255];

void handleRoot()
{
    JsonDocument jsonDocWT;
    jsonDocWT["Ma"] = WiFi.macAddress();
    String wData;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    SERIAL_PRINTLN(WiFi.macAddress());
}

void handleData()
{
    JsonDocument jsonDocWT;
    jsonDocWT["GC"] = WGC;
    jsonDocWT["WN"] = WWN;
    jsonDocWT["DB"] = WDB;
    jsonDocWT["ER"] = WER;
    jsonDocWT["RD"] = WRD;
    jsonDocWT["RH"] = WRH;
    jsonDocWT["RMi"] = WRMi;
    jsonDocWT["RMe"] = WRMe;
    String wData;
    serializeJson(jsonDocWT, wData);
    server.send(200, "text/json", wData);
    SERIAL_PRINTLN("WiFi Data Sent: " + wData);
}

void handleTime()
{
    JsonDocument jsonDocWT;
    String wData;
    if (server.hasArg("Va"))
    {
        String value = server.arg("Va");
        JsonDocument jsonDocWR;
        deserializeJson(jsonDocWR, value);
        wifiTime = jsonDocWR["Ti"].as<String>();
        SERIAL_PRINTLN("wifiTime: " + wifiTime);
        wT = true;
         wDB = true;
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
    JsonDocument jsonDocWT;
    String wData;
    if (server.hasArg("Va"))
    {
        String value = server.arg("Va");
        JsonDocument jsonDocWR;
        deserializeJson(jsonDocWR, value);
        wifiDisBuzzer = jsonDocWR["DB"];
        SERIAL_PRINTLN("wifiDisBuzzer: " + String(wifiDisBuzzer));
        wDB = true;
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

void handleScreen()
{
    JsonDocument jsonDocWT;
    String wData;
    if (server.hasArg("Va"))
    {
        String value = server.arg("Va");
        JsonDocument jsonDocWR;
        deserializeJson(jsonDocWR, value);
        wifiEnReminder = jsonDocWR["ER"];
        SERIAL_PRINTLN("wifiEnReminder: " + String(wifiEnReminder));
        wER = true;
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

void handleReminder()
{
    JsonDocument jsonDocWT;
    String wData;
    if (server.hasArg("Va"))
    {
        String value = server.arg("Va");
        JsonDocument jsonDocWR;
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

void handleSensor()
{
    JsonDocument jsonDocWT;
    String wData;
    if (server.hasArg("Va"))
    {
        String value = server.arg("Va");
        JsonDocument jsonDocWR;
        deserializeJson(jsonDocWR, value);
        wifilwarmupSensor = jsonDocWR["DB"];
         if (wifiDisBuzzer == 0)
        {
            buzzerBeepAck();
        }
        SERIAL_PRINTLN("wifilwarmupSensor: " + String(wifilwarmupSensor));
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
    if (ssid != nullptr && ssid[0] != '\0')
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            while (WiFi.status() != WL_CONNECTED)
            {
                wC = false;
                WiFi.setHostname("Sakha (C)");
                WiFi.begin(ssid, password);
                SERIAL_PRINTLN("Connecting to " + String(ssid) + ":" + String(password));
                vTaskDelay(30000 / portTICK_PERIOD_MS);
            }
            SERIAL_PRINT("Dynamic IP Address: ");
            SERIAL_PRINTLN(WiFi.localIP());
            if (!MDNS.begin("sakha-c"))
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
            server.on("/sensor", handleSensor);
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
