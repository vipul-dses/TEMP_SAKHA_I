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

#endif
