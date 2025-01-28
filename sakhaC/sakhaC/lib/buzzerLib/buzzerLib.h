#ifndef buzzerLib_h
#define buzzerLib_h

// Initialize the buzzer
void initializeBuzzer();

// Turn off the buzzer
void buzzerOff();

// Beep the buzzer for minor leak
void buzzerBeepL();

// Turn on the buzzer for major leak
void buzzerOn();

// Beep the buzzer for reminder
void buzzerBeepR();

void buzzerBeepAck();
#endif
