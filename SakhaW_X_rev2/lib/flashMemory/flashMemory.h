#ifndef flashmemory_h
#define flashmemory_h
#include <Arduino.h>
#include "SPIFFS.h"
#include "FS.h"

extern bool blcrFlag;
void initSPIFFS();
void readFile(fs::FS &fs, const char *path);
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void writeFile(fs::FS &fs, const char *path, const char *message);
void deleteFile(fs::FS &fs, const char *path);
void writeToSPIFFS();
void CRToSPIFFS();
void SPIFFStoCR();

#endif