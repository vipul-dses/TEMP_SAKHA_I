#ifndef FS_CONFIG_H_
#define FS_CONFIG_H_
#include <Arduino.h>
#include "mbfs/MB_MCU.h"

/**
 * To use other flash file systems
 *
 * LittleFS File system
 *
 * #include <LittleFS.h>
 * #define DEFAULT_FLASH_FS LittleFS //For ESP8266 or RPI2040 LitteFS
 *
 *
 * FAT File system
 *
 * #include <FFat.h>
 * #define DEFAULT_FLASH_FS FFat  //For ESP32 FAT
 *
 */
#if defined(ESP32)
#include <SPIFFS.h>
#endif
#if defined(ESP32) || defined(ESP8266)
#define DEFAULT_FLASH_FS SPIFFS
#elif defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_NANO_RP2040_CONNECT)
#include <LittleFS.h>
#define DEFAULT_FLASH_FS LittleFS
#endif

/**
 * To use SD card file systems with different hardware interface
 * e.g. SDMMC hardware bus on the ESP32
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD#faq
 *
 #include <SD_MMC.h>
 #define DEFAULT_SD_FS SD_MMC //For ESP32 SDMMC
 #define CARD_TYPE_SD_MMC 1 //For ESP32 SDMMC

 *
*/

/**
 * To use SdFat on ESP32

#if defined(ESP32)
#include <SdFat.h> // https://github.com/greiman/SdFat
static SdFat sd_fat_fs;   // should declare as static here
#define DEFAULT_SD_FS sd_fat_fs
#define CARD_TYPE_SD 1
#define SD_FS_FILE SdFile
#endif

* The SdFat (https://github.com/greiman/SdFat) is already implemented as wrapper class in ESP8266 and RP2040 core libraries.
* Do not include SdFat.h library in ESP8266 and RP2040 target codes which it conflicts with the wrapper one.

*/

#if defined(ESP32) || defined(ESP8266)
#include <SD.h>
#define DEFAULT_SD_FS SD
#define CARD_TYPE_SD 1
#elif defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_NANO_RP2040_CONNECT)
// Use SDFS (ESP8266SdFat) instead of SD
#include <SDFS.h>
#define DEFAULT_SD_FS SDFS
#define CARD_TYPE_SD 1
#endif

// For ESP32, format SPIFFS or FFat if mounting failed
#define FORMAT_FLASH_IF_MOUNT_FAILED 1

/** Use PSRAM for supported ESP32/ESP8266 module */
#if defined(ESP32) || defined(ESP8266)
#define ESP_SIGNER_USE_PSRAM
#endif

/* Enable NTP */
#define ESP_SIGNER_ENABLE_NTP_TIME

/* If not use on-board WiFi */
// #define ESP_SIGNER_DISABLE_ONBOARD_WIFI

/* If not use native Ethernet (Ethernet interfaces that supported by SDK) */
// #define ESP_SIGNER_DISABLE_NATIVE_ETHERNET

// For ESP8266 ENC28J60 Ethernet module
// #define ENABLE_ESP8266_ENC28J60_ETH

// For ESP8266 W5100 Ethernet module
// #define ENABLE_ESP8266_W5100_ETH

// For ESP8266 W5500 Ethernet module
// #define ENABLE_ESP8266_W5500_ETH

// To use your custom config, create Custom_FS_Config.h in the same folder of this FS_Config.h file
#if __has_include("Custom_FS_Config.h")
#include "Custom_FS_Config.h"
#endif


#endif