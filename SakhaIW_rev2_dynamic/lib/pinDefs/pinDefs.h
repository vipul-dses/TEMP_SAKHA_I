#ifndef pinDefs_h
#define pinDefs_h
#define LOADCELL_DOUT_PIN 23  //16 tested pins for old pcb
#define LOADCELL_SCK_PIN 19   //4

#define REDLED 2
#define GRNLED 4
#define BLULED 16
#define BUZ 14
#define BAT_AN 34
#define BAT_TRG 16
#define batteryPin 34  // ADC1 channel pin (e.g., GPIO 34)
#define batGPIO 32

void pinInit();
// Comment the following line to disable serial print
#define ENABLE_SERIAL_PRINT

#ifdef ENABLE_SERIAL_PRINT
#define WEIGHT_PRINT(...) Serial.print(__VA_ARGS__)     // Macro for printing without newline
#define WEIGHT_PRINTLN(...) Serial.println(__VA_ARGS__) // Macro for printing with newline
#define BATTERY_LOG(...) Serial.print(__VA_ARGS__)     // Macro for printing without newline
#define BATTERY_LOGLN(...) Serial.println(__VA_ARGS__) // Macro for printing with newline
#else
#define SERIAL_PRINT(...)     // Empty macro when serial print is disabled
#define SERIAL_PRINTLN(...)   // Empty macro when serial print is disabled
#endif

#endif
