#include "Arduino.h"
#include "displayLib.h"
#include "SH1106Wire.h"
#include "SPI.h"

SH1106Wire display(0x3c, SDA, SCL);

#define DEMO_DURATION 3000
typedef void (*Demo)(void);
int demoMode = 0;
int counter = 1;

bool iS = false;
bool iW = false;
int iR;
int GC;
String HR;
String ME;
String DY;
String DE;
String MH;
String RM;

#define Bluetooth_width 12
#define Bluetooth_height 12
const uint8_t Bluetooth_Logo[] PROGMEM = {0x00, 0x00, 0x20, 0x00, 0xe0, 0x00, 0xa0, 0x01, 0xa8, 0x00, 0x70, 0x00, 0x70, 0x00, 0xa8, 0x00,
                                          0xa0, 0x01, 0xe0, 0x00, 0x20, 0x00, 0x00, 0x00};

#define WiFi_width 12
#define WiFi_height 12
const uint8_t WiFi_Logo[] PROGMEM = {0x00, 0x00, 0x60, 0x00, 0xfc, 0x03, 0x03, 0x0c, 0xf9, 0x09, 0x0c, 0x03, 0x60, 0x00, 0x90, 0x00,
                                     0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

void initializeDisplay()
{
    display.init();
    display.flipScreenVertically();
}

void screenOne()
{
    if (!iS)
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Your safety companion");
        display.setFont(Arimo_Regular_24);
        display.drawString(64, 15, "Sakha");
        display.setFont(Arimo_Regular_18);
        display.drawString(64, 42, "DSES Pvt Ltd");
        display.display();
    }
    else if (iS && GC < 2)
    {
        display.clear();
        if (iW)
        {
            display.drawXbm(0, 0, WiFi_width, WiFi_height, WiFi_Logo);
        }
        display.drawXbm(116, 0, Bluetooth_width, Bluetooth_height, Bluetooth_Logo);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, DY + ", " + DE + " " + MH);
        display.setFont(Arimo_Regular_24);
        display.drawString(64, 15, HR + ":" + ME);
        display.setFont(Arimo_Regular_18);
        display.drawString(64, 42, "GC: " + String(GC) + " PPM");
        display.display();
    }
    else if (iS && GC >= 2 && GC < 5)
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Notification");
        display.setFont(Arimo_Regular_24);
        display.drawString(64, 15, String(GC) + " PPM");
        display.setFont(Arimo_Regular_18);
        display.drawString(64, 42, "Low Gas Leak");
        display.display();
    }
    else if (iS && GC >= 5)
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Notification");
        display.setFont(Arimo_Regular_24);
        display.drawString(64, 15, String(GC) + " PPM");
        display.setFont(Arimo_Regular_18);
        display.drawString(64, 42, "High Gas Leak");
        display.display();
    }
}

void screenTwo()
{
    if (GC < 2 && iR == 0)
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Safety Tip");
        String textLines[] = {
            "Place cylinder upright on flat ground.",
            "Verify cylinder weight upon delivery.",
            "Check cylinder for seal, leaks, expiry.",
            "Handle cylinders carefully to prevent mishaps.",
            "Maintain parts regularly for safety.",
            "Keep stove on raised platform.",
            "Turn off regulator after cooking.",
            "Wear apron while cooking meals.",
            "Avoid storing cylinders in enclosed spaces.",
            "Keep cylinders away from flammable items.",
            "Store spare cylinders separately.",
            "Use only undamaged hoses for safety.",
            "Use approved methods for leak detection.",
            "Never leave cooking unattended.",
            "Avoid curtains near gas stove.",
            "Avoid handling utensils using clothing.",
        };
        static int currentLineIndex = 0;
        String currentText = textLines[currentLineIndex];
        display.drawStringMaxWidth(64, 15, 128, currentText);
        display.display();
        delay(DEMO_DURATION);
        currentLineIndex = (currentLineIndex + 1) % (sizeof(textLines) / sizeof(String));
    }
    else if (GC < 2 && iR == 1)
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Reminder");
        display.drawStringMaxWidth(64, 15, 128, RM);
        display.display();
    }
    else if (GC >= 2)
    {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.setFont(Arimo_Regular_12);
        display.drawString(64, 0, "Gas Leak Safety Tip");
        String textLines[] = {
            "Avoid switching ON/OFF electrical fittings indoors.",
            "Avoid lighting matches, lighters, etc.",
            "Open all doors and windows immediately.",
            "If regulator is ON, switch to OFF immediately.",
            "Remove regulator & fix safety cap on valve.",
            "Remove cylinder and contact Distributor/DO.",
        };
        static int currentLineIndex = 0;
        String currentText = textLines[currentLineIndex];
        display.drawStringMaxWidth(64, 15, 128, currentText);
        display.display();
        delay(DEMO_DURATION);
        currentLineIndex = (currentLineIndex + 1) % (sizeof(textLines) / sizeof(String));
    }
}

Demo demos[] = {screenOne, screenTwo};
int demoLength = sizeof(demos) / sizeof(Demo);
long timeSinceLastModeSwitch = 0;

void monitorDisplay()
{
    demos[demoMode]();
    if (millis() - timeSinceLastModeSwitch > DEMO_DURATION)
    {
        demoMode = (demoMode + 1) % demoLength;
        timeSinceLastModeSwitch = millis();
    }
    counter++;
    delay(10);
}