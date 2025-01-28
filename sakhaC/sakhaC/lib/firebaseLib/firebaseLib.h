#ifndef firebaselib_h
#define firebaselib_h

#include <Arduino.h>

String createGasLeakJsonPayload(uint8_t gasConcentration);
void sendFCMMessage(String jsonPayload);

#endif
