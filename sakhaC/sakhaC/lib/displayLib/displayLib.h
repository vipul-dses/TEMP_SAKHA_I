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

// This function sets up the necessary configurations and parameters for the display.
void initializeDisplay();

// This function updates the display content and handles display-related operations.
void monitorDisplay();

#endif
