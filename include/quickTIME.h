#ifndef QUICKTIME_H
#define QUICKTIME_H

#include "quicklib.h"

byte DoTimer();
void DoRealTime();
void RTC_GetTemp();
long RTC_GetDateTime();
char RTC_SetDateTime();
byte IsLeapYear(uint16_t yearIN);
byte GetWeekDay(uint32_t serialTime);
uint32_t SerializeTime(byte dayIN, byte monthIN, uint16_t yearIN, byte hourIN, byte minIN, byte secIN);
byte GetDaysOfMonth(char monthIN, uint16_t yearIN);
void DeSerializeTime(uint32_t serializedIN, byte *dayIN, byte *monthIN, uint16_t *yearIN, byte *hourIN, byte *minIN, byte *secIN);
uint32_t StrToTime(char *timeIN);
uint32_t StrToDate(char *dateIN);
void SerialDateToStr(uint32_t timeIN);
void SerialDayTimeToStr(uint32_t timeIN);

void DayTimeDateToStr(byte hourDay, byte minuteMonth, uint16_t secondYear, char delimiterChar, byte lastLen);
#define DayTimeToStr(hour, minute, second) DayTimeDateToStr(hour, minute, second, ':', 2)
#define DateToStr(day, month, year) DayTimeDateToStr(day, month, year, '.', 4)


// Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals

// global RunTime Timing
  extern byte myRunSec;
  extern byte myRunMin;
  extern byte myRunHour;
  extern uint16_t myRunDay;
  extern uint32_t myRunTime;

// global RealTime Timing
  extern byte mySec;
  extern byte myMin;
  extern byte myHour;
  extern byte myDay;
  extern byte myMonth;
  extern uint16_t myYear;
  extern uint32_t myTime;


#endif
