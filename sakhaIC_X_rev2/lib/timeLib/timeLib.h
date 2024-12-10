#ifndef timeLib_h
#define timeLib_h

extern String cHour;
extern String cMinute;
extern String cDate;
extern String cMonth;
extern String cDay;
extern String cTime;
extern int rDay;
extern int rHour;
extern int rMinute;
extern String sHour;

// Initialize the time
void initializeTime();


uint32_t getUnix();


// Monitor the time
void monitorTime();


// Update the time
void updateTime();

#endif