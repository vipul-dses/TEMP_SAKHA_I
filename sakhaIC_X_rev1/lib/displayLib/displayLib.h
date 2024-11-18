#ifndef displayLib_h
#define displayLib_h

extern bool iS;
extern bool iW;
extern int iR;
extern int GC;
extern String HR;
extern String ME;
extern String DY;
extern String DE;
extern String MH;
extern String RM;
extern bool iE;
extern float TW;
extern float CW;
extern int GP;
extern float GW;
extern bool stopScroll;
// This function sets up the necessary configurations and parameters for the display.
void initializeDisplay();
void bleAck(String, float);
// This function updates the display content and handles display-related operations.
void monitorDisplay();

#endif