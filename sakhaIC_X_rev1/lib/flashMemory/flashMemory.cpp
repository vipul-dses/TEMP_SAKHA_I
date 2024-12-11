#include "flashMemory.h"
#include "ArduinoJson.h"
#include <Preferences.h>
#include "bleCom.h"
#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include "espCom.h"
#include "sensorLib.h"
#include "config.h"
#include "wifiCom.h"
#include "WiFi.h"
#include "WebServer.h"
#include "WiFiUdp.h"

#define MAX_ENTRIES 100
Preferences sPreferences;
String totalWeightstr;
int indexDecimal;
const int buttonPin = 0;
int fileCounter = 0;
int fileNumber;
String dumpFilePath;
String myFilePath;
#include "timeLib.h"
#define FORMAT_SPIFFS_IF_FAILED true
float gasWeigh = 0.0;
uint32_t gasTime = 0;
/***************************************list directories ***************************************/
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println("- not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
      Serial.println("  CONTENTS:");

      while (file.available())
      {
        String data = "";
        for (int i = 0; i < 50 && file.available(); ++i)
        {
          data += (char)file.read(); // Accumulate up to 50 bytes
        }
        if (bGraph)
        {
          pTxCharacteristic->setValue(data.c_str());
          pTxCharacteristic->notify();
          Serial.println("Bl Data sent: " + data);
        }
        if (wGraph)
        {
          server.send(200, "text/json", data);
          Serial.println("Wi-Fi Data sent: " + data);
        }
      }
    }
    file = root.openNextFile();
  }

  // Send "$$" after all files are processed
  String endMarker = "$$";
  if (bGraph)
  {
    pTxCharacteristic->setValue(endMarker.c_str());
    pTxCharacteristic->notify();
    Serial.println("Bl Data sent: $$");
  }
  if (wGraph)
  {
    server.send(200, "text/json", endMarker);
    Serial.println("Wi-Fi Data sent: $$");
  }
}

/*****
void listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{

  Serial.printf("Listing directory: %s\r\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory())
  {
    Serial.println(" - not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
      Serial.println("  CONTENTS:");
    //   while (file.available())
    //   {
    //     Serial.write(file.read()); // Read and print each byte
    // }
      while (file.available()) {
                String data = "";
                for (int i = 0; i < 50 && file.available(); ++i) {
                    data += (char)file.read();  // Accumulate up to 20 bytes
                }
if(bGraph)
{
     pTxCharacteristic->setValue(data.c_str());
        pTxCharacteristic->notify();
        Serial.println("Bl Data sent: " + data);
       }
       if(wGraph)
       {
        server.send(200, "text/json", data);
        Serial.println("wifi Data sent: " + data);

  }

      }
    file = root.openNextFile();
  }
}
}
*/
/***************************************read file ***************************************/
void readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if (!file || file.isDirectory())
  {
    Serial.println("- failed to open file for reading");
    return;
  }

  Serial.println("- read from file:");
  while (file.available())
  {
    Serial.write(file.read());
  }
  file.close();
}

/***************************************write file ***************************************/

void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}
/***************************************delete file ***************************************/

void deleteFile(fs::FS &fs, const char *path)
{
  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path))
  {
    Serial.println("- file deleted");
  }
  else
  {
    Serial.println("- delete failed");
  }
}
void initSPIFFS()
{
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
}

void writeToSPIFFS()
{

  if (true)
  {
    delay(50);
    sPreferences.begin("mD", false);
    fileCounter = sPreferences.getInt("eC", fileCounter);
    Serial.println("Loaded counter: " + String(fileCounter));
    // counter=0;
    fileCounter++;
    if (fileCounter > MAX_ENTRIES)
    {
      fileCounter = 1;
    }
    Serial.println(fileCounter);
    sPreferences.begin("mD", false);
    sPreferences.putInt("eC", fileCounter);
    sPreferences.end();
    Serial.println("Updated counter: " + String(fileCounter));

    myFilePath = "/" + String(fileCounter) + ".txt";

    const char *filePath = myFilePath.c_str();

    gasTime = getUnix();
    gasWeigh = monitorCom(); // random(1, 30);
    totalWeightstr = String(gasWeigh);
    indexDecimal = totalWeightstr.indexOf('.');
    //         Serial.print("index is : ");
    // Serial.println(indexDecimal);
    totalWeightstr = totalWeightstr.substring(0, indexDecimal + 2);
    ////Serial.print("before conversion: ");
    // Serial.println(totalWeight);
    // Serial.print("after conversion: ");
    // Serial.println(totalWeightstr);
    fileNumber = fileCounter;
    DynamicJsonDocument jsonDocSPIF(512);
    jsonDocSPIF["UT"] = gasTime;
    jsonDocSPIF["SW"] = totalWeightstr;
    jsonDocSPIF["FN"] = fileNumber;
    jsonDocSPIF["BP"] = incomingbatteryVoltage;
    String sData;
    serializeJson(jsonDocSPIF, sData);

    const char *sDatastr = sData.c_str();
    Serial.print("JSON::::: ");
    Serial.println(sDatastr);
    writeFile(SPIFFS, filePath, sDatastr);
    // readFile(SPIFFS, filePath);

    // deleteFile(SPIFFS, "/0.txt");
  }
}

void appendFile(fs::FS &fs, const char *path, const char *message)
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

void CRToSPIFFS()
{
  //  deleteFile(SPIFFS, "/hello.txt");
  Serial.println(testString);
  testString += ',';
  const char *testStr = testString.c_str();
  appendFile(SPIFFS, "/hello.txt", testStr);
  //  readFile(SPIFFS, "/hello.txt");

  crDataflag = false;
}

void SPIFFStoCR()
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
  pTxCharacteristic->setValue(data4.c_str());
  pTxCharacteristic->notify();
  // Serial.println("CR Data sent: " + data4);

  blcrFlag = false;
}
