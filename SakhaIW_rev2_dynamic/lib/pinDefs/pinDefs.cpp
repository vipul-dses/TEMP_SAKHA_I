#include "Arduino.h"
#include "pinDefs.h"

void pinInit()
{

    pinMode(BUZ, OUTPUT);
    pinMode(batGPIO, OUTPUT);
    digitalWrite(batGPIO, HIGH);
    // digitalWrite(REDLED, HIGH);
    // digitalWrite(GRNLED, HIGH);
    // digitalWrite(BLULED, LOW);

    digitalWrite(BUZ, LOW);
}