#ifndef bleCom_h
#define bleCom_h

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

// Initialize BLE communication
void initializeBle();

// Send data using BLE communication
void monitorBle();

#endif
