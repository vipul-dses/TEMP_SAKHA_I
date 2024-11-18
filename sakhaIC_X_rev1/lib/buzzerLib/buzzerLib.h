#ifndef buzzerLib_h
#define buzzerLib_h

// Initialize the buzzer
void initializeBuzzer();

// Turn off the buzzer
void buzzerOff();

// Turn on the buzzer
void buzzerOn();

// Beep the buzzer for leak
void buzzerBeepL();

// Beep the buzzer for alarm
void buzzerBeepR();

// Beep the buzzer for runout
void buzzerRunout();

void buzzerBeepAck();

#endif