#include "Arduino.h"
#include "config.h"
#include "buzzerLib.h"
#include "Ticker.h"

Ticker buzzer;

void initializeBuzzer()
{
    pinMode(BUZZER, OUTPUT);
    digitalWrite(BUZZER, LOW);
}

void buzzerOff()
{
    digitalWrite(BUZZER, LOW);
    SERIAL_PRINTLN("Buzzer off");
}

void buzzerOffCallback()
{
    digitalWrite(BUZZER, LOW);
    buzzer.detach();
}
void buzzerBeep(float duration)
{
    digitalWrite(BUZZER, HIGH);
    buzzer.attach(duration, buzzerOffCallback);
}
void buzzerBeepAck()
{
    buzzerBeep(0.05);
    SERIAL_PRINTLN("Buzzer beep leak");
}
// void buzzerBeep(float duration)
// {
//     digitalWrite(BUZZER, HIGH);
//     buzzer.attach(duration, buzzerOffCallback);
// }

void buzzerBeepL()
{
    buzzerBeep(0.6);
    SERIAL_PRINTLN("Buzzer beep leak");
}

void buzzerOn()
{
    digitalWrite(BUZZER, HIGH);
    SERIAL_PRINTLN("Buzzer on");
}

void buzzerBeepR()
{
    buzzerBeep(0.03);
    SERIAL_PRINTLN("Buzzer beep reminder");
}