#ifndef sensorLib_h
#define sensorLib_h
extern float weight;
extern int gasPercentage;
extern float gasWeight;
extern int regulatorMode;
extern const float calibrationFactor;    //111171.11; // Sakha-I 1variant ///home unit 66903.18
extern const long zeroFactor;  
extern bool sensorFlag;
void sleepSensor();
float getBatteryPercentage();
float readSensor();
#endif