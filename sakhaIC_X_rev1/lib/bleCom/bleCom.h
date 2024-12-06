#ifndef bleCom_h
#define bleCom_h

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"

extern BLEServer *pServer;
extern BLECharacteristic *pTxCharacteristic;
extern int BGC;
extern String BWN;
extern int BDB;
extern int BER;
extern int BRD;
extern int BRH;
extern int BRMi;
extern String BRMe;
extern String wifiName;
extern String wifiPass;
extern bool bNP;
extern String blTime;
extern bool bT;
extern int blDisBuzzer;
extern bool bDB;
extern int blEnReminder;
extern bool bER;
extern int blRemDay;
extern int blRemHour;
extern int blRemMinute;
extern String blRemMessage;
extern bool bR;
extern int blwarmupSensor;
extern bool bWS;
extern float BTW;
extern float BCW;
extern int BRMo;
extern int blRegulatorMode;
extern bool bRM;
extern float blContainerWeight;
extern bool bCW;
extern bool bGraph;


extern bool blcrFlag;
extern bool crDataflag;
extern String testString;

// Initialize BLE communication
void initializeBle();
void bleAck();
// Send data using BLE communication
void monitorBle();

#endif