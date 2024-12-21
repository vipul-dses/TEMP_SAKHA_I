#include <otaLib.h>
#include <Preferences.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <Update.h>

#define SERVICE_UUID "2e9141d9-9a00-4c86-b512-8134b0e43070"
#define RX_BEGIN_SIZE "6b59e69e-da9f-4ab3-96f5-61aba4015e66"
#define RX_FIRMWARE "0709e586-629e-40c2-afd2-fb9c2ab32422"
#define RX_END "8cd4e413-99d0-4000-bdc4-07f5092f9a08"

#define OTA_BEGIN "ota_begin"
#define OTA_END "ota_end"

#define UPDATE_CHUNK_SIZE 512

#define PREFS_NAME "ota"
#define KEY_OTA "ota"

bool prefsInitialized = false;
Preferences prefs;

BLEServer *otaBLEServer = NULL;
BLECharacteristic *txCharacteristic;

size_t totalSize = 0;
size_t receivedSize = 0;

void setOTA(bool value);
void handleOTAData(std::string data);
void restartESP();

class OTAServerCallback : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        Serial.println("OTA: BLE Device connected");
    }
    void onDisconnect(BLEServer *pServer)
    {
        Serial.println("OTA: BLE Device disconnected");
    }
};

void debugPrintHex(const uint8_t *data, size_t length)
{
    Serial.print("Received data (hex): ");
    for (size_t i = 0; i < length; i++)
    {
        Serial.printf("%02X ", data[i]);
    }
    Serial.println();
}

class BeginSizeCallback : public BLECharacteristicCallbacks
{

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        uint8_t *data = pCharacteristic->getData();
        size_t length = pCharacteristic->getLength();

        // Serial.println("Received packet:");
        // debugPrintHex(data, length);

        if (value.length() < 1)
        {
            return;
        }

        totalSize = (data[3] << 24) |
                    (data[2] << 16) |
                    (data[1] << 8) |
                    data[0];
        // Begin OTA update
        Serial.println("OTA: Begin");
        // Serial.println("OTA: Total Size: " + totalSize);
        if (!Update.begin(totalSize))
        {
            Serial.println("OTA: Result: Failure");
            Update.printError(Serial);
            restartESP();
        }
    }
};

class FirmwareCallback : public BLECharacteristicCallbacks
{

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        uint8_t *data = pCharacteristic->getData();
        size_t length = pCharacteristic->getLength();

        // Serial.println("Received packet:");
        // debugPrintHex(data, length);

        if (value.length() < 1)
        {
            return;
        }

        // Write data
        size_t writtenBytes = Update.write(data, length);

        // Debug write info
        Serial.print("Written bytes: ");
        Serial.print(writtenBytes);
        Serial.print(" of ");
        Serial.println(length);

        // if (writtenBytes != length)
        if (Update.hasError())
        {
            Serial.println("Write failed!");
            Update.printError(Serial);
            Update.abort();
            restartESP();
            return;
        }

        receivedSize += writtenBytes;
        Serial.print("Received: ");
        Serial.print(receivedSize);
        Serial.print(" / ");
        Serial.println(totalSize);
    }
};

class EndCallback : public BLECharacteristicCallbacks
{

    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();
        uint8_t *data = pCharacteristic->getData();
        size_t length = pCharacteristic->getLength();

        // Serial.println("Received packet:");
        // debugPrintHex(data, length);

        if (value.length() < 1)
        {
            return;
        }

        if (Update.end(true))
        {
            Serial.println("OTA: Result: Success");
        }
        else
        {
            Serial.println("OTA: Result: Failure");
            Update.printError(Serial);
        }
        restartESP();
    }
};

void restartESP()
{
    setOTA(false);
    ESP.restart();
}

void initializeBLE()
{
    BLEDevice::init("Sakha (I)");

    BLEDevice::setMTU(517);

    otaBLEServer = BLEDevice::createServer();
    otaBLEServer->setCallbacks(new OTAServerCallback());

    BLEService *pService = otaBLEServer->createService(SERVICE_UUID);

    BLECharacteristic *beginSizeCharacteristic = pService->createCharacteristic(RX_BEGIN_SIZE, BLECharacteristic::PROPERTY_WRITE);
    beginSizeCharacteristic->setCallbacks(new BeginSizeCallback());

    BLECharacteristic *firmwareCharacteristic = pService->createCharacteristic(RX_FIRMWARE, BLECharacteristic::PROPERTY_WRITE);
    firmwareCharacteristic->setCallbacks(new FirmwareCallback());

    BLECharacteristic *endCharacteristic = pService->createCharacteristic(RX_END, BLECharacteristic::PROPERTY_WRITE);
    endCharacteristic->setCallbacks(new EndCallback());

    pService->start();

    otaBLEServer->getAdvertising()->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    Serial.println("OTA: BLE server started");
}

void initPrefs()
{
    if (!prefsInitialized)
    {
        prefs.begin(PREFS_NAME);
        prefsInitialized = true;
    }
}

void setOTA(bool value)
{
    initPrefs();
    prefs.putBool(KEY_OTA, value);
    prefs.end();
}

bool getIsOTA()
{
    initPrefs();
    return prefs.getBool(KEY_OTA, false);
}

bool checkAndStartOTA(std::string data)
{
    if (data.length() < 1)
    {
        return false;
    }
    if (data == OTA_BEGIN)
    {
        Serial.println("OTA: Restart ESP");
        setOTA(true);
        // BLEDevice::deinit(true);
        ESP.restart();
        return true;
    }
    return false;
}

bool checkAndResumeOTA()
{
    bool isOTA = getIsOTA();
    if (!isOTA)
    {
        return false;
    }
    setOTA(false);
    initializeBLE();
    return true;
}
