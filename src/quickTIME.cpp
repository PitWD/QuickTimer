#include "quickTIME.h"



byte DoTimer(){
  
  static uint32_t last_mS = 0;
  static uint32_t lost_mS = 0;
  uint32_t left_mS = 0;

  uint32_t sys_mS = millis();

  // Time left since last call
  if (last_mS > sys_mS){
    // millis() overflow
    left_mS = (uint32_t)4294967295 - last_mS + sys_mS;
  }
  else{
    left_mS = sys_mS - last_mS;
  }

  // add lost mS from last call
  left_mS += lost_mS;
  
  // at least a second is over...
  if (left_mS >= 1000){

    // save actual time for next call
    last_mS = sys_mS;

    // get the lost time
    lost_mS = left_mS - 1000;

    // Calculate myRunTime
    myRunTime++;
    myRunSec++;
    if (myRunSec == 60){
      myRunSec = 0;
      myRunMin++;
    }
    if (myRunMin == 60){
      myRunMin = 0;
      myRunHour++;
    }
    if (myRunHour == 24){
      myRunHour = 0;
      myRunDay++;
    }
    
    // Calculate myRealTime
    DoRealTime();
    return 1;
  }
  
  return 0;
  
}

void DoRealTime(){
  // need to get called every second...

  //Trigger for RTC sync
  #if USE_RTC
    static uint32_t triggerRTC = 0;
    static uint32_t rtcSyncDiff = 0;
  #endif
  
  // Overflow day (default 32 = more months with 31 days)
  char overflowDay = 32;

  mySec++;
  myTime++;
  
  if (mySec == 60){
    mySec = 0;
    myMin++;
  }
  if (myMin == 60){
    myMin = 0;
    myHour++;
  }
  if (myHour == 24){
    // Midnight Jump...
    myHour = 0;
    myDay++;
    
    if (myMonth == 2){
      // February
      if (myDay > 28){
        if (!IsLeapYear(myYear)){
          // It's not a leap-year
          myDay = 1;
          myMonth = 3;
        }        
      }
      else if(myDay > 29){
        // It's a leap-year
        myDay = 1;
        myMonth = 3;
      }
    }
    else{
      // Regular months
      switch (myMonth){
      case 4:
      case 6:
      case 9:
      case 11:
        // 30 days
        overflowDay = 31;
      default:
        if (myDay == overflowDay){
          myDay = 1;
          myMonth++;
        }
        break;
      }
    }
    if (myMonth == 13){
      myMonth = 1;
      myYear++;
    }    
  }
  #if USE_RTC
    if (syncRTCinterval){
      if (!triggerRTC){
        // sync with RTC
        rtcSyncDiff = RTC_GetDateTime();
        if (rtcSyncDiff){
          // Update stuff which has 'LastActionTime" dependencies...
        }
        // reset trigger
        triggerRTC = syncRTCinterval;
      }
      triggerRTC--;
    }
  #endif
}

#if USE_RTC

void RTC_GetTemp(){
  IIcSetBytes(0x68, (char*)"\x11", 1);
  if (IIcGetBytes(0x68, 2) == 2){
    // Full Read success
    myRtcTemp = iicStr[0] * 1000;
    myRtcTemp += (iicStr[1] >> 6) * 250;
  }
}

uint32_t RTC_GetDateTime(){
  
  uint32_t r = 0;

  IIcSetBytes(0x68, (char*)"\0", 1);
  if (IIcGetBytes(0x68, 7) == 7){
    // Full Read Succeed
    mySec = FromBCD(iicStr[0]);
    myMin = FromBCD(iicStr[1]);
    myHour = FromBCD(iicStr[2]);
    myDay = FromBCD(iicStr[4]);
    myMonth = FromBCD(iicStr[5]);
    myYear = FromBCD(iicStr[6]) + 2000;

    if (myYear < 2023){
      myYear = 2023;
    }
    
    r = myTime;    // save for diff calculation   
    myTime = SerializeTime(myDay, myMonth, myYear, myHour, myMin, mySec);
    r -= myTime;

  }
  return r;   // positive = µC is faster 
}

char RTC_SetDateTime(){
  
  char r = 0;

  strHLP[0] = 0;
  strHLP[1] = ByteToChar(ToBCD(mySec));
  strHLP[2] = ByteToChar(ToBCD(myMin));
  strHLP[3] = ByteToChar(ToBCD(myHour));
  r = IIcSetBytes(0x68, strHLP, 4);
    
  if (r > 0){
    strHLP[0] = 4;
    strHLP[1] = ByteToChar(ToBCD(myDay));
    strHLP[2] = ByteToChar(ToBCD(myMonth));
    strHLP[3] = ByteToChar(ToBCD((byte)(myYear - 2000)));
    r = IIcSetBytes(0x68, strHLP, 4);

  }

  return r;

}

#endif

byte IsLeapYear(uint16_t yearIN){
  if (!(yearIN % 4) && (yearIN % 100)){
    // Leap
    return 1;
  }
  else if(!(yearIN % 100) && (yearIN % 400)){
    // No Leap
    return 0;
  }
  else if(!(yearIN % 400)){
    // Leap
    return 1;
  }
  else{
    // No Leap
    return 0;
  }
}

uint32_t SerializeTime(byte dayIN, byte monthIN, uint16_t yearIN, byte hourIN, byte minIN, byte secIN){
    // Do Serialized Time (Start from 01.01.2023)
    uint32_t serializedTime = 0;
    // Years
    for (uint16_t i = 2023; i < yearIN; i++){
      serializedTime += SECS_IN_YEAR; // 365 * 86400
      if (IsLeapYear(i)){
        serializedTime += SECS_IN_DAY;
      }
    }
    // Months
    for (byte i = 1; i < monthIN; i++){
      serializedTime += (uint32_t)GetDaysOfMonth(i, yearIN) * SECS_IN_DAY;
    }
    // Days
    serializedTime += (uint32_t)(dayIN - 1) * SECS_IN_DAY;
    // Hours
    serializedTime += (uint32_t)hourIN * SECS_IN_HOUR;
    // Minutes
    serializedTime += (uint32_t)minIN * SECS_IN_MIN;
    // Seconds
    return serializedTime + (uint32_t)secIN;
}

byte GetDaysOfMonth(byte monthIN, uint16_t yearIN){

  byte r = 31;

  // Regular months
  switch (monthIN){
  case 2:
    r = 28;
    if (IsLeapYear(yearIN)){
      r++;
    }
    break;
  case 4:
  case 6:
  case 9:
  case 11:
    r = 30;
  default:
    // r = 31;
    break;
  }

  return r;
}

byte GetWeekDay(uint32_t serialTime){
  // 01.01.2023 (start of 'myTime') was a Sunday
  // Sunday is day 1

  byte r = 1;
  uint32_t hlp = serialTime % SECS_IN_WEEK;    // remove full weeks
  
  r += (byte)(hlp / SECS_IN_DAY);                   // add full days
  if (hlp % SECS_IN_DAY){
    r++;                                      // one more...    
  }
  
  return r;

}

void DeSerializeTime(uint32_t serializedIN, byte *dayIN, byte *monthIN, uint16_t *yearIN, byte *hourIN, byte *minIN, byte *secIN){

  uint32_t nextSeconds = SECS_IN_YEAR;
  *yearIN = 2023;

  // Year
  while (serializedIN >= nextSeconds){
    // actual year is full
    
    *yearIN += 1;
    serializedIN -= nextSeconds;
    
    nextSeconds = SECS_IN_YEAR;
    if (IsLeapYear(*yearIN)){
      nextSeconds += SECS_IN_DAY;
    }
    
  }
  
  // Month
  nextSeconds = (uint32_t)2678400;
  *monthIN = 1;
  while (serializedIN >= nextSeconds){
    *monthIN += 1;
    serializedIN -= nextSeconds;
    nextSeconds = (uint32_t)GetDaysOfMonth(*monthIN, *yearIN) * SECS_IN_DAY;
  }

  // Day
  *dayIN = (byte)(serializedIN / SECS_IN_DAY) + 1;
  serializedIN = serializedIN % SECS_IN_DAY;

  // Hour
  *hourIN = (byte)(serializedIN / SECS_IN_HOUR);
  serializedIN = serializedIN % SECS_IN_HOUR;

  // Min
  *minIN = (byte)(serializedIN / SECS_IN_MIN);

  // Sec
  *secIN = (byte)(serializedIN % SECS_IN_MIN);

}

uint32_t StrToTime(char *timeIN){
    char *token;
    int i = 0;
    byte hour = 0;
    byte minute = 0;
    byte second = 0;
    token = strtok(timeIN, ":");
    while (token != NULL && i < 3) {
        byte t = atoi(token);
        if (t < 0 || t > 59) {
            return 0;
        }
        if (i == 0 && t > 23) {
            return 0;
        }
        if (i == 0) {
            hour = t;
        } else if (i == 1) {
            minute = t;
        } else if (i == 2) {
            second = t;
        }
        token = strtok(NULL, ":");
        i++;
    }
    if (i != 3) {
        return 0;
    }
    return SerializeTime(1, 1, 2023, hour , minute , second);
}

uint32_t StrToDate(char *dateIN){
    char *token;
    int i = 0;
    byte day = 0;
    byte month = 0;
    uint16_t year = 0;
    token = strtok(dateIN, ".");
    while (token != NULL && i < 3) {
        int t = atoi(token);
        if (t < 0 ) {
            return 0;
        }
        if (i == 0 && t > 31) {
            return 0;
        }
        if (i == 0) {
            day = t;
        } else if (i == 1) {
            month = t;
        } else if (i == 2) {
            year = t;
        }
        token = strtok(NULL, ".");
        i++;
    }
    if (i != 3) {
        return 0;
    }
    return SerializeTime(day, month, year, 0 , 0 ,0);
}

void DayTimeDateToStr(byte hourDay, byte minuteMonth, uint16_t secondYear, char delimiterChar, byte lastLen){
  IntToIntStr(hourDay, 2, '0');
  strcpy(strHLP2, strHLP);
  strHLP2[2] = delimiterChar;
  IntToIntStr(minuteMonth, 2, '0');
  strcpy(strHLP2 + 3, strHLP);
  strHLP2[5] = delimiterChar;
  IntToIntStr(secondYear, lastLen, '0');
  strcpy(strHLP2 + 6, strHLP);
  strHLP2[6 + lastLen] = 0;
}

void SerialDateToStr(uint32_t timeIN){
  byte day = 0;
  byte month = 0;
  uint16_t year = 0;
  DeSerializeTime(timeIN, &day, &month, &year, NULL, NULL, NULL);
  DateToStr(day, month, year);
}

void SerialDayTimeToStr(uint32_t timeIN){
  byte hour = 0;
  byte minute = 0;
  byte second = 0;
  DeSerializeTime(timeIN, NULL, NULL, NULL, &hour, &minute, &second);
  DayTimeToStr(hour, minute, second);
}
