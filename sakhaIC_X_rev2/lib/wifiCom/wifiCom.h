#ifndef wifiCom_h
#define wifiCom_h
#include "WebServer.h"
extern WebServer server;
extern const char *ssid;
extern const char *password;
extern int WGC;
extern String WWN;
extern int WDB;
extern int WER;
extern int WRD;
extern int WRMo;
extern float WTW;
extern float WCW;
extern int WRH;
extern int WRMi;
extern String WRMe;
extern bool wC;
extern String wifiTime;
extern bool wT;
extern int wifiDisBuzzer;
extern bool wDB;
extern int wifiEnReminder;
extern bool wER;
extern int wifiRemDay;
extern int wifiRemHour;
extern int wifiRemMinute;
extern String wifiRemMessage;
extern bool wR;
extern int wifilwarmupSensor;
extern bool wWS;
extern float containerWeight;
extern float totalWeight;
extern int regulatorMode;
extern bool wGraph;
extern bool wRM;
extern int wRegulatorMode;
extern float wContainerWeight;
extern bool wCW;
extern bool wcrFlag;
// Monitor Wi-Fi communication
void monitorWiFi();

#endif