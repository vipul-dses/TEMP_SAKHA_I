#include "Arduino.h"
#include "config.h"
#include "timeLib.h"
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char monthsOfTheYear[13][12] = {" ", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

String cHour;
String cMinute;
String cDate;
String cMonth;
String cDay;
String cTime;
int rDay;
int rHour;
int rMinute;
String sHour;

void initializeTime()
{
    if (!rtc.begin())
    {
        TIME_PRINTLN("Couldn't find RTC");
    }
}

void monitorTime()
{
    DateTime now = rtc.now();
    cHour = String(now.twelveHour());
    cMinute = (now.minute() < 10) ? "0" + String(now.minute()) : String(now.minute());
   // TIME_PRINTLN(cHour + ":" + cMinute);

    cDate = String(now.day());
    cMonth = String(monthsOfTheYear[now.month()]);
    cDay = String(daysOfTheWeek[now.dayOfTheWeek()]);
  //  TIME_PRINTLN(cDate + " " + cMonth + ", " + cDay);

    rDay = now.dayOfTheWeek();
    rHour = now.hour();
    rMinute = now.minute();
 //   TIME_PRINTLN(String(rDay) + ", " + String(rHour) + ":" + String(rMinute));

    bool isPM = now.isPM();
    if (isPM || (rHour >= 8 && rHour <= 11))
    {
        sHour = "PM";
      //  TIME_PRINTLN("PM");
    }
    else
    {
        sHour = "AM";
     //   TIME_PRINTLN("AM");
    }
}

void updateTime()
{
    int year, month, date, hour, minute, second;
    sscanf(cTime.c_str(), "%d,%d,%d,%d,%d,%d", &year, &month, &date, &hour, &minute, &second);
    DateTime newTime(year, month, date, hour, minute, second + 2);
    rtc.adjust(newTime);
}

uint32_t getUnix()
{
    DateTime now = rtc.now();
  //    TIME_PRINTLN(" since midnight 1/1/1970 = ");
  //  TIME_PRINTLN(now.unixtime());
 //  TIME_PRINTLN("s = ");
 //   TIME_PRINTLN(now.unixtime() / 86400L);
    return now.unixtime();
}