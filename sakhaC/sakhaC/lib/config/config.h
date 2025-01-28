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

#endif
