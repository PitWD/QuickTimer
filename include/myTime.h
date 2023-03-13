#include <Arduino.h>
#include <EEPROM.h>

#define RUNNING_TIMERS_CNT 16

struct TimerSTRUCT{
  uint32_t onTime[3];
  uint32_t offTime[3];
  uint32_t offset[3];
  struct typeUNION{
    byte interval     :1;
    byte doubleI      :1;
    byte tripleI      :1;
    byte dayTimer     :1;
    byte invert       :1;
  }type;
  byte weekDays;
    // 0 = all
    // 2nd Bit = Sunday
    // 3rd Bit = Monday
    // 4th...
  struct stateUNION{
    byte automatic    :1;
    byte permOff      :1;
    byte permOn       :1;
    byte tempOn       :1;
    byte tempOff      :1;
    byte lastVal      :1;
    byte hasChanged   :1;
  }state;
  uint32_t tempUntil;
  char name[17];
}runningTimer;

struct TimerStateSTRUCT{
  struct stateUNION{
    byte automatic    :1;
    byte permOff      :1;
    byte permOn       :1;
    byte tempOn       :1;
    byte tempOff      :1;
    byte lastVal      :1;
    byte hasChanged   :1;
  }state;
  uint32_t tempUntil;
}runningState[RUNNING_TIMERS_CNT];

// global RunTime Timing
  byte myRunSec = 0;
  byte myRunMin = 0;
  byte myRunHour = 0;
  uint16_t myRunDay = 0;
  byte myRunTime = 0;

// global RealTime Timing
  byte mySec = 0;
  byte myMin = 0;
  byte myHour = 0;
  byte myDay = 1;
  byte myMonth = 1;
  uint16_t myYear = 2023;
  uint32_t myTime = 0;

// RTC-Temp
long myRtcTemp = 0;

// Seconds between RTC sync's (0 disables sync
#define syncRTCinterval 86400L

void TimerFromRomRam(byte timer, byte ram){
  // Load Timer
  EEPROM.get(timer * sizeof(TimerSTRUCT), runningTimer);
  if (ram){
    // copy temporay stuff from ram, too
    //runningTimer.state.permOff = runningState[timer].state.permOff;
    //runningTimer.state.permOn = runningState[timer].state.permOn;
    runningTimer.state.tempOff = runningState[timer].state.tempOff;
    runningTimer.state.tempOn = runningState[timer].state.tempOn;
    runningTimer.state.lastVal = runningState[timer].state.lastVal;
    runningTimer.state.hasChanged = runningState[timer].state.hasChanged;
    runningTimer.state.automatic = runningState[timer].state.automatic;
    runningTimer.tempUntil = runningState[timer].tempUntil;
  }
}

void TimerToRomRam(byte timer, byte rom){
  // Move the temporary stuff to ram
  runningState[timer].state.permOff = runningTimer.state.permOff;
  runningState[timer].state.permOn = runningTimer.state.permOn;
  runningState[timer].state.tempOff = runningTimer.state.tempOff;
  runningState[timer].state.tempOn = runningTimer.state.tempOn;
  runningState[timer].state.lastVal = runningTimer.state.lastVal;
  runningState[timer].state.hasChanged = runningTimer.state.hasChanged;
  runningState[timer].state.automatic = runningTimer.state.automatic;
  runningState[timer].tempUntil = runningTimer.tempUntil;
  if (rom){
    // Save the lasting stuff to eeprom, too...
    runningTimer.state.tempOff = 0;
    runningTimer.state.tempOn = 0;
    runningTimer.state.lastVal = 0;
    runningTimer.state.hasChanged = 0;
    runningTimer.tempUntil = 0;
    EEPROM.put(timer * sizeof(TimerSTRUCT), runningTimer);
  }
}

char ByteToChar(byte valIN){
    // Keeps the Bit-Pattern but NOT the VALUE
    if (valIN > 127){
        return (char)(valIN - 256);
    }
    return (char)valIN;
}

byte GetWeekDay(uint32_t serialTime){
  // 01.01.2023 (start of 'myTime') was a Sunday
  // Sunday is day 1

  byte r = 1;
  uint32_t hlp = serialTime % 604800;    // remove full weeks
  
  r += (byte)(hlp / 86400);                   // add full days
  if (hlp % 86400){
    r++;                                      // one more...    
  }
  
  return r;

}

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

byte GetDaysOfMonth(char monthIN, uint16_t yearIN){

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

uint32_t SerializeTime(byte dayIN, byte monthIN, uint16_t yearIN, byte hourIN, byte minIN, byte secIN){
    // Do Serialized Time (Start from 01.01.2023)
    uint32_t serializedTime = 0;
    // Years
    for (uint16_t i = 2023; i < yearIN; i++){
      serializedTime += 31536000; // 365 * 86400
      if (IsLeapYear(i)){
        serializedTime += 86400;
      }
    }
    // Months
    for (int i = 1; i < monthIN; i++){
      serializedTime += (uint32_t)GetDaysOfMonth(i, yearIN) * 86400UL;
    }
    // Days
    serializedTime += (uint32_t)(dayIN - 1) * 86400UL;
    // Hours
    serializedTime += (uint32_t)hourIN * 3600;
    // Minutes
    serializedTime += (uint32_t)minIN * 60;
    // Seconds
    return serializedTime + secIN;
}

void DeSerializeTime(uint32_t serializedIN, byte *dayIN, byte *monthIN, uint16_t *yearIN, byte *hourIN, byte *minIN, byte *secIN){

  uint32_t nextSeconds = 31536000UL;
  *yearIN = 2023;

  // Year
  while (serializedIN > nextSeconds){
    // actual year is full
    
    *yearIN += 1;
    serializedIN -= nextSeconds;
    
    nextSeconds = 3153600UL;
    if (IsLeapYear(*yearIN)){
      nextSeconds += 86400UL;
    }
    
  }
  
  // Month
  nextSeconds = 2678400UL;
  *monthIN = 1;
  while (serializedIN > nextSeconds){
    *monthIN += 1;
    serializedIN -= nextSeconds;
    nextSeconds = GetDaysOfMonth(*monthIN, *yearIN) * 86400UL;
  }

  // Day
  *dayIN = (serializedIN / 86400) + 1;
  serializedIN = serializedIN % 86400;

  // Hour
  *hourIN = serializedIN / 3600;
  serializedIN = serializedIN % 3600;

  // Min
  *minIN = serializedIN / 60;

  // Sec
  *secIN = serializedIN % 60;

}

byte ToBCD (byte val){
  return ((val / 10) << 4) + (val % 10);
}
byte FromBCD (byte bcd){
  return (10 * ((bcd & 0xf0) >> 4)) + (bcd & 0x0f);
}

void RTC_GetTemp(){
  IIcSetBytes(0x68, (char*)"\x11", 1);
  if (IIcGetBytes(0x68, 2) == 2){
    // Full Read success
    myRtcTemp = iicStr[0] * 1000;
    myRtcTemp += (iicStr[1] >> 6) * 250;
  }
}

long RTC_GetDateTime(){
  
  long r = 0;

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

char IntToStr(long val, char lz, byte dp, char lc){

    // dp = decimal places
    // lz = leading zero's
    // lc = leading char for zero
    // return = position of decimal point

    // int (scaled by 1000)
    ltoa(val, strHLP, 10);
    byte len = strlen(strHLP);

    if (len < 4){
        // value is < 1 (1000)
        memmove(&strHLP[4 - len], &strHLP[0], len);
        memset(&strHLP[0], '0', 4 - len);
        len = 4;
    }
    
    // Set leading zero's
    lz -= (len - 3);
    if (lz > 0){
        // space for missing zeros
        memmove(&strHLP[(int)lz], &strHLP[0], len);
        // set missing zeros
        memset(&strHLP[0], lc, lz);
        // correct len
        len += lz;        
    }

    // shift dp's to set decimal point
    memmove(&strHLP[len -2], &strHLP[len - 3], 3);
    // set decimal point
    strHLP[len - 3] = '.';
    len++;

    // Trailing zero's
    lz = dp + lz - len + 2;
    if (lz > 0){
        // missing trailing zero's
        memset(&strHLP[len], '0', lz);
        len += lz;
    }

    // Return final decimal point
    lz = len - 4;

    // calculate decimal places
    if (dp > 0){
        // cut the too much dp's
        len -= 3 - dp;
    }
    else if (dp == 0){
        // integer
        len = lz;
        lz = 0;
    }

    // set EndOfString
    strHLP[len] = 0;

    return lz;
}

uint32_t StrToTime(char *timeIN){
    char *token;
    int i = 0;
    byte hour = 0;
    byte minute = 0;
    byte second = 0;
    token = strtok(timeIN, ":");
    while (token != NULL && i < 3) {
        int t = atoi(token);
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
    return ((long)hour * 3600) + (minute * 60) + (second);
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

void DayTimeToStr(byte hour, byte minute, byte second){
  IntToStr((long)hour * 1000, 2, 0, '0');
  strcpy(strHLP2, strHLP);
  strHLP2[2] = ':';
  IntToStr((long)minute * 1000, 2, 0, '0');
  strcpy(strHLP2 + 3, strHLP);
  strHLP2[5] = ':';
  IntToStr((long)second * 1000, 2, 0, '0');
  strcpy(strHLP2 + 6, strHLP);
  strHLP2[8] = 0;
}

void DateToStr(byte day, byte month, uint16_t year){
  IntToStr((long)day * 1000, 2, 0, '0');
  strcpy(strHLP2, strHLP);
  strHLP2[2] = '.';
  IntToStr((long)month * 1000, 2, 0, '0');
  strcpy(strHLP2 + 3, strHLP);
  strHLP2[5] = '.';
  IntToStr((long)year * 1000, 4, 0, '0');
  strcpy(strHLP2 + 6, strHLP);
  strHLP2[10] = 0;
}

void PrintHlpTime(byte hourIN, byte minIN, byte secIN){
    DayTimeToStr(hourIN, minIN, secIN);
    Serial.print(strHLP2);
}

void PrintHlpDate(byte dayIN, byte monthIN, uint16_t yearIN){
    DateToStr(dayIN, monthIN, yearIN);
    Serial.print(strHLP2);
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

void PrintSerTime(uint32_t timeIN, byte printDays){
  // if "printDays" is set, '000d 00:00:00'
  // else         '00:00:00'

  if (printDays){
    uint32_t days = timeIN / 86400UL;
    IntToStr(days * 1000, 3, 0, ' ');
    Serial.print(strHLP);
    Serial.print(F("d "));
  }
  SerialDayTimeToStr(timeIN);
  Serial.print(strHLP2);
}

void PrintTime(){
    PrintHlpTime(myHour, myMin, mySec);
}

void PrintRunTime(){
    PrintSerTime(myRunTime, 1);
}

void PrintDateTime(){
    PrintHlpDate(myDay, myMonth, myYear);
    Serial.print(F(" "));
    PrintHlpTime(myHour, myMin, mySec);
}

void DoRealTime(){
  // need to get called every second...

  //Trigger for RTC sync
  static uint32_t triggerRTC = 0;
  static long rtcSyncDiff = 0;

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
}

byte DoTimer(){
  
  static uint32_t last_mS = 0;
  static uint32_t lost_mS = 0;
  uint32_t left_mS = 0;

  uint32_t sys_mS = millis();

  // Time left since last call
  if (last_mS > sys_mS){
    // millis() overflow
    left_mS = 4294967295UL - last_mS + sys_mS;
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

// TIMER

uint32_t CurrentIntervalPos(uint32_t timerIN, uint32_t onTime, uint32_t offTime, uint32_t offset){
  // Calculate the current position within the interval
  //return (uint32_t)(timerIN - offset) % (uint32_t)(onTime + offTime);
  return (timerIN - offset) % (onTime + offTime);
}
#define LastInterval(timerIN, onTime, offTime, offset) CurrentIntervalPos(timerIN, onTime, offTime, offset)

byte IntervalTimer(uint32_t timerIN, uint32_t onTime, uint32_t offTime, uint32_t offset) {
    // Check if the current position is within the "on" interval
    if (CurrentIntervalPos(timerIN, onTime, offTime, offset) < onTime){
        return 1; // "on" interval
    }
    return 0;     // "off" interval
}

uint32_t NextInterval(uint32_t timerIN, uint32_t onTime, uint32_t offTime, uint32_t offset){

  return (onTime + offTime) - CurrentIntervalPos(timerIN, onTime, offTime, offset);

}

byte CalcIntervalTimer(uint32_t timerIN){

  byte r = 0;
  uint32_t currentPos = CurrentIntervalPos(timerIN, runningTimer.onTime[0], runningTimer.offTime[0], runningTimer.offset[0]);
  // "runningTimers" is already loaded

  // Check if 1st interval is valid
  //if (IntervalTimer(timerIN, runningTimer.onTime[0], runningTimer.offTime[0], runningTimer.offset[0])){
  if (currentPos < runningTimer.onTime[0]){
    r = 1;
    // Check if Double or Triple Timer is active
    if (runningTimer.type.doubleI || runningTimer.type.tripleI){
      // Check if 2nd Interval during OnTime is valid
      if (currentPos >= runningTimer.offset[1]){
        if (IntervalTimer(currentPos, runningTimer.onTime[1], runningTimer.offTime[1], runningTimer.offset[1])){
          // still ON
        }
        else{
          r = 0;
        }
      }
      else{
        r = 0;
      }
    }
  }
  else{
    // Check if Triple Timer is active
    if (runningTimer.type.tripleI){
      // Check if 3rd Interval during off-time is active
      if (currentPos - runningTimer.onTime[0] >= runningTimer.offset[2]){
        if (IntervalTimer(currentPos - runningTimer.onTime[0], runningTimer.onTime[2], runningTimer.offTime[2], runningTimer.offset[2])){
          r = 1;
        }
      }      
    }    
  }  
  return r;
}

byte DayTimer (uint32_t timerIN, uint32_t onTime, uint32_t offTime){

  // ordinary 24h timer

  uint32_t onDuration;

  if (offTime < onTime){
    // Jump over Midnight
    onDuration = (86400L - onTime) + offTime;
  }
  else{
    onDuration = offTime - onTime;
  }
  
  uint32_t offDuration = 86400L - onDuration;

  return IntervalTimer(timerIN, onDuration, offDuration, onTime);

}

// Returns the state of the bit at position 'bitToGet' in 'byteIN'
byte getBit(byte byteIN, byte bitToGet) {
    return (byteIN >> bitToGet) & 0x01;
}

// Sets the bit at position 'bitToSet' in 'byteIN' to 'setTo' 
uint8_t setBit(byte byteIN, byte bitToSet, byte setTo) {
    if (setTo) {
        byteIN |= (1 << bitToSet);
    } else {
        byteIN &= ~(1 << bitToSet);
    }
    return byteIN;
}

byte RunTimers(){
  // Returns true if one ore more states have changed

  byte r = 0; // Helper
  byte r2 = 0;

  for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){

    // Load Timer
    TimerFromRomRam(i, 1);
    
    if (runningTimer.type.dayTimer){
      // 24h DayTimer...
      r = DayTimer(myTime, runningTimer.onTime[0], runningTimer.offTime[0]);
    }
    else if (runningTimer.type.interval){
      // Interval Timer(s)
      r = CalcIntervalTimer(myTime);
    }
    // Check on valid weekdays
    if (!getBit(runningTimer.weekDays, GetWeekDay(myTime)) && runningTimer.weekDays){
      // Day is not valid
      r = 0;
    }
    // Check on permanent & temporary state
    if (runningTimer.state.permOff){
      r = 0;
    }
    else if (runningTimer.state.permOn){
      r = 1;
    }
    else if (runningTimer.state.tempOff && myTime < runningTimer.tempUntil){
      r = 0;
    }
    else if (runningTimer.state.tempOn && myTime < runningTimer.tempUntil){
      r = 1;
    }
    else if (runningTimer.state.automatic){
      // r = r;
    }
    else{
      // Totally disabled
      r = 0;
    }
    // Clear Temp-Times and States if possible
    if (runningTimer.tempUntil && (myTime >= runningTimer.tempUntil)){
      runningTimer.tempUntil = 0;
      runningTimer.state.tempOn = 0;
      runningTimer.state.tempOff = 0;
    }
    // Invert if needed
    if (runningTimer.type.invert){
      r = !r;
    }
    // Check on change
    if (r != runningTimer.state.lastVal){
      // Is changed
      runningTimer.state.lastVal = r;
      runningTimer.state.hasChanged = 1;
      r2 = 1;
    }
    else{
      runningTimer.state.hasChanged = 0;
    }
    // Move the temporary stuff to ram
    TimerToRomRam(i, 0);
  }
  return r2;

}