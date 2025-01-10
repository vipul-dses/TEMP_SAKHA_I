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

#define MAX_ENTRIES 720
Preferences sPreferences;

#include "timeLib.h"
#define FORMAT_SPIFFS_IF_FAILED true

const int buttonPin = 0;
int fileCounter = 0;
int fileNumber;
String dumpFilePath;
String myFilePath;

float gasWeigh = 0.0;
uint32_t gasTime = 0;
String totalWeightstr;
int indexDecimal;
int fileIndexCr;
int lastIndex;

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
    // Check if the file name is in the range 1000.txt to 1050.txt
    String fileName = file.name();
    if (fileName.endsWith(".txt"))
    {
      // Extract the numeric part of the filename
      int lastSlash = fileName.lastIndexOf('/');
      int dotIndex = fileName.lastIndexOf('.');
      String baseName = fileName.substring(lastSlash + 1, dotIndex); // Numeric part only

      // Convert to integer
      int fileNumber = baseName.toInt();

      // Skip if fileNumber is in the range 1000 to 1050
      if (fileNumber >= 1000 && fileNumber <= 1050)
      {
        Serial.print("Skipping file: ");
        Serial.println(fileName);
        file = root.openNextFile(); // Move to the next file
        continue;                   // Skip processing for this file
      }
    }

    // // Skip hello.txt
    // if (strcmp(file.name(), "hello.txt") == 0)
    // {
    //   file = root.openNextFile();
    //   continue;
    // }
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
          //        Serial.println("Bl Data sent: " + data);
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

/***************************************append file ***************************************/

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

/***************************************SPIFFS Intialization ***************************************/

void initSPIFFS()
{
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
}
/***************************************************************************/
// Writing Gas consumption data into SPIFFS received from SAKHA-IW via ESP-NOW
void writeToSPIFFS()
{
  if (true)
  {
    delay(50);
    sPreferences.begin("eC", false);
    fileCounter = sPreferences.getInt("eC", fileCounter);
    Serial.println("Loaded counter: " + String(fileCounter));
    // counter=0;
    fileCounter++;
    if (fileCounter > MAX_ENTRIES)
    {
      fileCounter = 1;
    }
    Serial.println(fileCounter);
    sPreferences.begin("eC", false);
    sPreferences.putInt("eC", fileCounter);
    sPreferences.end();
    Serial.println("Updated counter: " + String(fileCounter));

    myFilePath = "/" + String(fileCounter) + ".txt";

    const char *filePath = myFilePath.c_str();

    gasTime = getUnix();
    gasWeigh = monitorCom();
    totalWeightstr = String(gasWeigh);
    indexDecimal = totalWeightstr.indexOf('.');
    totalWeightstr = totalWeightstr.substring(0, indexDecimal + 2);
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
/******************************************************************/
// Store cylinder records into SPIFFS which is received from App
void CRToSPIFFS()
{
  //  deleteFile(SPIFFS, "/hello.txt");
  sPreferences.begin("eC", false);
  fileIndexCr = sPreferences.getInt("eC", fileIndexCr);
  if (fileIndexCr == 0)
  {
    fileIndexCr = 1000;
  }
  Serial.print("fileCounter:");
  Serial.println(fileIndexCr);
  Serial.println("received CR data.");
  Serial.println(testString);
  testString += ',';
  const char *testStr = testString.c_str();
  String myfileIndexStr = "/" + String(fileIndexCr) + ".txt";
  const char *fileIndexStr = myfileIndexStr.c_str();
  writeFile(SPIFFS, fileIndexStr, testStr);
  // appendFile(SPIFFS, "/hello.txt", testStr);
  //  readFile(SPIFFS, "/hello.txt");
  readFile(SPIFFS, fileIndexStr);
  fileIndexCr = fileIndexCr + 1;
  sPreferences.begin("eC", false);
  sPreferences.putInt("eC", fileIndexCr);
  sPreferences.end();
  testString = ""; // Clear the string
  Serial.println("testString has been reset for new data.");
  Serial.println(testString);

  crDataflag = false;
}


void WCRToSPIFFS(String data)
{
  testString=data;
 sPreferences.begin("eC", false);
  fileIndexCr = sPreferences.getInt("eC", fileIndexCr);
    Serial.print("Get wiFi fileCounter:");
  Serial.println(fileIndexCr);
    sPreferences.end();

  if (fileIndexCr == 0)
  {
    fileIndexCr = 1000;
  }

  Serial.println("received CR data.");
  Serial.println(testString);
  testString += ',';
  const char *testStr = testString.c_str();
  String myfileIndexStr = "/" + String(fileIndexCr) + ".txt";
  const char *fileIndexStr = myfileIndexStr.c_str();
  writeFile(SPIFFS, fileIndexStr, testStr);
  // appendFile(SPIFFS, "/hello.txt", testStr);
  //  readFile(SPIFFS, "/hello.txt");
  readFile(SPIFFS, fileIndexStr);
  fileIndexCr = fileIndexCr + 1;
  sPreferences.begin("eC", false);
  sPreferences.putInt("eC", fileIndexCr);
  sPreferences.end();
    Serial.print("Set wiFi fileCounter:");
  Serial.println(fileIndexCr);
  testString = ""; // Clear the string
  Serial.println("testString has been reset for new data.");
  Serial.println(testString);

  crDataflag = false;
}
/**************************************************************************************************************/
// Retrieve cylinder record from SPIFFS and sends to app
void SPIFFStoCR()
{
  Serial.println("CR Transfer function");
  String data1 = "";
  String data2 = "{\"Re\": 1, \"data\": [";
  for (int crFile = 1000; crFile <= 1050; crFile++)
  {
    String filename = "/" + String(crFile) + ".txt"; // Generate filename
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
    //  Serial.println("Data sent: " + data1);
    int data1Len = data1.length();
    Serial.println("File length: " + String(data1Len));
    data1.remove(data1.length() - 1);
    String data3 = "]}";
    String data4 = data2 + data1 + data3;
    // Serial.println("CR Data sent: " + data4);
    file.close();
    if (data1Len == 0)
    {
      crFile = 1051;
      blcrFlag = false;
      wcrFlag = false;
    }
    if (blcrFlag)
    {
      pTxCharacteristic->setValue(data4.c_str());
      pTxCharacteristic->notify();
      Serial.println("CR Data sent: " + data4);
      data4 = "";
      
    }
    // if (wcrFlag)
    // {
    //   server.send(200, "text/json", data4);
    //   Serial.println("Wi-Fi Data sent print: " + data4);
    //   data4 = "";  
    // }
    data1 = "";
    data1Len = 0;
  }
}
