#include "Arduino.h"
#include "config.h"
#include "bleCom.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include "ArduinoJson.h"
#include "buzzerLib.h"
#include "espCom.h"
#include "displayLib.h"
#include "otaLib.h"
//BGC variable stores gas concentration in task3Code via monitorSensor() function
int BGC;
//BWN variable stores WiFi name in task3Code received via BLE App
String BWN;
//BDB variable stores buzzer status in task3Code received via BLE App
int BDB;
//BER variable stores reminder status in task3Code received via BLE App
int BER;
//BRD variable stores reminder date in task3Code received via BLE App
int BRD;
//BRH variable stores reminder hour in task3Code received via BLE App
int BRH;
//BRMi variable stores reminder hour in task3Code received via BLE App
int BRMi;
//BRMe variable stores reminder message in task3Code received via BLE App
String BRMe;
//BTW variable stores total weight in task3Code which is received from Sakha-IW via ESP-NOW
float BTW;
//BCW variable stores container weight in task3Code received via BLE App
float BCW;
//BRMo variable stores regulator mode status in task3Code received via BLE App
int BRMo;
String wifiName;
String wifiPass;
//nNP flag indicates WiFi credentials received in BLE mode
bool bNP = false;
//blTime variable stores updated sync time received from BLE app
String blTime;
//Bt flag indicates sync time command received from BLE app 
bool bT = false;
//blDisBuzzer variable store buzzer status received from BLE app
int blDisBuzzer;
//bDB flag indicates buzzer modification received from BLE app 
bool bDB = false;
//blEnReminder variable stores value reminder status received from BLE app
int blEnReminder;
//bER flag indicates enable status is modified from BLE app
bool bER = false;

int blRemDay;
int blRemHour;
int blRemMinute;
String blRemMessage;
bool bR = false;
int blwarmupSensor;
bool bWS = false;

int blRegulatorMode;
bool bRM = false;
float blContainerWeight;
bool bCW = false;

int blGetGraph;
bool bGraph;
bool blcrFlag;
String testString;
bool crDataflag;

extern int trolleyStatus;

BLEServer *pServer = NULL;
BLECharacteristic *pTxCharacteristic;
int deviceConnected = 0;
#define SERVICE_UUID "018dda31-e845-76a5-8a25-50c037092ab9"
#define CHARACTERISTIC_UUID_RX "018dda33-099e-7906-bd75-05afa51d3164"
#define CHARACTERISTIC_UUID_TX "018dda33-4afe-7fdb-94fa-6aeb182982c7"

class SakhaCServerCallback : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected++;
       // buzzerBeepAck();
        SERIAL_PRINTLN("BLE Device connected");
    }
    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected--;
        SERIAL_PRINTLN("BLE Device disconnected");
    }
};

class SakhaCCharacteristicsCallback : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string characteristicValue = pCharacteristic->getValue();
        bool isOTA = checkAndStartOTA(characteristicValue);
        if (isOTA)
        {
            return;
        }
        if (characteristicValue.length() > 0)
        {
            DynamicJsonDocument jsonDocBR(1024);
            deserializeJson(jsonDocBR, characteristicValue);
            int type = jsonDocBR["Ty"];
            switch (type)
            {
            case 0:
                wifiName = jsonDocBR["WN"].as<String>();
                wifiPass = jsonDocBR["WP"].as<String>();
                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }
                bNP = true;
                SERIAL_PRINTLN("Received data for namePass: " + wifiName + " " + wifiPass);
                break;
            case 1:
                blTime = jsonDocBR["Ti"].as<String>();
                bT = true;
                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }
                SERIAL_PRINTLN("Received data for blTime: " + blTime);
                break;
            case 2:
                blDisBuzzer = jsonDocBR["DB"];
                bDB = true;

                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }

                //  buzzerBeepAck();
                SERIAL_PRINTLN("Received data for blDisBuzzer: " + String(blDisBuzzer));
                break;
            case 3:
                blEnReminder = jsonDocBR["ER"];
                bER = true;
                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }
                SERIAL_PRINTLN("Received data for blEnReminder: " + String(blEnReminder));
                break;
            case 4:
                blRemDay = jsonDocBR["RD"];
                blRemHour = jsonDocBR["RH"];
                blRemMinute = jsonDocBR["RMi"];
                blRemMessage = jsonDocBR["RMe"].as<String>();
                bR = true;
                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }
                SERIAL_PRINTLN("Received data for dHMM: " + String(blRemDay) + " " + String(blRemHour) + " " + String(blRemMinute) + " " + blRemMessage);
                break;
            case 5:
                blwarmupSensor = jsonDocBR["WS"];

                SERIAL_PRINTLN("Received data for blwarmupSensor: " + String(blwarmupSensor));

                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }
                if (!bWS)
                {
                    bWS = true;

                    SERIAL_PRINTLN("blwarmupSensor Start");
                }
                break;
            case 6:
                blRegulatorMode = jsonDocBR["RMo"];
                bRM = true;
                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }

                SERIAL_PRINTLN("Received data for blRegulatorMode: " + String(blRegulatorMode));
                break;
            case 7:
                blContainerWeight = jsonDocBR["CW"];
                if (blDisBuzzer == 0)
                {
                    buzzerBeepAck();
                }
                bCW = true;
                SERIAL_PRINTLN("Received data for blContainerWeight: " + String(blContainerWeight));
                break;
            case 8:
                blGetGraph = jsonDocBR["SF"];

                bGraph = true;
                Serial.println("Received data for blContainerWeight: " + String(blGetGraph));
                break;
            case 9: // store cylinder records to SPIFFS
                serializeJson(jsonDocBR, testString);
                //  Serial.println(testString);
                crDataflag = true;
                // SERIAL_PRINTLN("Received data for CR: " + blCRCT + " " + blCROP + " " + String(blCRCW) + " " + String(blCRGW)+ " " +blCRDD+ " " + String(blCRAD));
                break;
            case 10: // Send cylinder record from SPIFFS to APP
                blcrFlag = true;
                Serial.println("Request for cylinder records: " + String(blcrFlag));
                break;    
            }
        }
        characteristicValue.clear();
    }
};

void initializeBle()
{
    BLEDevice::init("Sakha (I)");
    BLEDevice::setMTU(517);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new SakhaCServerCallback());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new SakhaCCharacteristicsCallback());
    pService->start();
    pServer->getAdvertising()->start();
    SERIAL_PRINTLN("BLE server started");
}

void monitorBle()
{
    if (deviceConnected > 0)
    {
        DynamicJsonDocument jsonDocBT(1024);
        jsonDocBT["GC"] = BGC;
        jsonDocBT["WN"] = BWN;
        jsonDocBT["DB"] = BDB;
        jsonDocBT["ER"] = BER;
        jsonDocBT["RD"] = BRD;
        jsonDocBT["RH"] = BRH;
        jsonDocBT["RMi"] = BRMi;
        jsonDocBT["RMe"] = BRMe;

        jsonDocBT["TW"] = BTW;
        jsonDocBT["CW"] = BCW;
        jsonDocBT["RMo"] = BRMo;
        jsonDocBT["BP"] = incomingbatteryVoltage;
        jsonDocBT["TS"] =trolleyStatus;
        String bData;
        serializeJson(jsonDocBT, bData);
        pTxCharacteristic->setValue(bData.c_str());
        pTxCharacteristic->notify();
        SERIAL_PRINTLN("Bl Data sent: " + bData);
        delay(10);
    }
    pServer->startAdvertising();
}
