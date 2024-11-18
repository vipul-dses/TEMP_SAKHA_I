#ifndef config_h
#define config_h
 
// Pin Definitions
#define MQ6 36
#define BUZZER 27
#define RED 25
#define GREEN 33
#define BLUE 32

// Comment the following line to disable serial print
#define ENABLE_SERIAL_PRINT
#ifdef ENABLE_SERIAL_PRINT
#define SERIAL_PRINT(...) Serial.print(__VA_ARGS__)
#define SERIAL_PRINTLN(...) Serial.println(__VA_ARGS__)

#else
#define SERIAL_PRINT(...)
#define SERIAL_PRINTLN(...)
#endif

//#define ENABLE_GAS_PRINT
#ifdef ENABLE_GAS_PRINT
#define GAS_PRINT(...) Serial.print(__VA_ARGS__)
#define GAS_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define GAS_PRINT(...)
#define GAS_PRINTLN(...)
#endif

//#define ENABLE_TIME_PRINT
#ifdef ENABLE_TIME_PRINT
#define TIME_PRINT(...) Serial.print(__VA_ARGS__)
#define TIME_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define TIME_PRINT(...)
#define TIME_PRINTLN(...)
#endif



#define ENABLE_BLE_PRINT
#ifdef ENABLE_BLE_PRINT
#define BLE_PRINT(...) Serial.print(__VA_ARGS__)
#define BLE_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define BLE_PRINT(...)
#define BLE_PRINTLN(...)
#endif

#define ENABLE_NOW_PRINT
#ifdef ENABLE_NOW_PRINT
#define NOW_PRINT(...) Serial.print(__VA_ARGS__)
#define NOW_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define NOW_PRINT(...)
#define NOW_PRINTLN(...)
#endif

#endif

