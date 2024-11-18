#ifndef sensorLib_h
#define sensorLib_h

extern int gasConc;
extern int gasConc;
extern int regulatorMode;
extern float gasWeight;
extern float dGW;
extern float cGW;
extern float gP;
extern int gasPercentage;
extern float containerWeight;
// Initialize the sensor
void initializeSensor();
float monitorCom();
// Monitor the sensor
void monitorSensor();

#endif