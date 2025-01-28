#ifndef otalib_h
#define otalib_h

#include <Arduino.h>

bool checkAndStartOTA(std::string data);
bool checkAndResumeOTA();

#endif
