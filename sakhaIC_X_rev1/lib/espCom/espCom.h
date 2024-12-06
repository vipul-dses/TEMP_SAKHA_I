#ifndef espCom_h
#define espCom_h

extern float totalWeight;
extern bool eD;
extern int eDCounter;
extern int incomingbatteryVoltage;
// Initialize ESP-NOW communication
void initializeCom();

// Store ESP-NOW Data
void eData();

#endif
