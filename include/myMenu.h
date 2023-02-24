
#include "myIIC.h"
#include "myTime.h"
#include <EEPROM.h>

byte myBoot = 0;    // 0 = Terminal  /  1 = Slave
uint32_t mySpeed = 9600;

long exp10(int e){
  long x = 1;
  for (int i = 0; i < e; i++) {
    x = x * 10;
  }
  return x;
}

long StrToInt(char *strIN, byte next){

    // "1.234" , 1000  ==> 1234
    // mul == 0 search next

    long r = 0;

    static char *nextVal = NULL;
    static char *actVal = NULL;

    long preDot = 0;
    long afterDot = 0;
    
    if (next){
        // Next Val
        actVal = nextVal;
    }
    else{
        // New Val
        actVal = strIN;
    }

    preDot = atol(actVal) * 1000;

    // decimal dot
    char *dot = strchr(actVal, '.');
    
    // (probably) next number
    nextVal = strchr(actVal, ',');
    
    if (dot){
        if (nextVal == NULL){
            nextVal = strchr(actVal, '\0');
        }
        // count of missing digits after dot
        r = 3 - (long)(nextVal - dot);
        r = exp10(r + 1);
        for (nextVal -= 1; nextVal > dot; nextVal--){
            afterDot += r * (nextVal[0] - 48);
            r *= 10;
        }
    }
    if (nextVal){
        nextVal++;
    }

    if (preDot >= 0){
        r = preDot + afterDot;
    }
    else{
        r = preDot - afterDot;
    }

    return r;
}

void PrintErrorOK(char err, char ezo, char *strIN){

  // Err: 0 = info, -1 = err, 1 = OK
  // Err: 0 = black, -1 = red, 1 = green

  byte len = strlen(strIN) + 40;

  EscInverse(1);
  EscLocate(1,24);

  if (err == -1){
    // Error
    EscColor(bgRed);
  }
  else if(err == 1){
    // OK
    EscColor(bgGreen);
  }
  else{
    // Info
    EscBold(1);
  }
  
  Serial.print(F("    "));
  Serial.print(strIN);
  EscBold(0);
  EscColor(49);

  Serial.print(F(" @ "));
  PrintRunTime();

  len = 80 - len;
  for (int i = 0; i < len; i++){
    Serial.print(F(" "));
  }
  
  PrintDateTime();
  EscInverse(0);

}

byte GetUserString(char *strIN){
  
  char c = 0;
  byte timeOut = 60;
  byte eos = 0;       // Pos of EndOfString

  EscBold(1);
  Serial.print(F(">> "));
  EscBold(0);
  EscColor(fgCyan);

  eos = strlen(strIN);
  if (eos){
    Serial.print(strIN);
    strcpy(strHLP, strIN);
  }
  
  strHLP[eos] = 0;

  while (c != 13){

    if (DoTimer()){
      timeOut--;
      if (!timeOut){
        strHLP[0] = 0;
        EscColor(0);
        return 0;
      }
    }

    if (Serial.available()){
      
      timeOut = 60;

      c = Serial.read();
      if (eos < IIC_HLP_LEN - 1){
        eos++;
      }
      else{
        EscCursorLeft(1);
      }

      switch (c){
      case 8:
      case 27:
      case 127:
        // DEL and Backspace
        if (eos > 1){
          eos -= 2;
          EscCursorLeft(1);
          Serial.print(F(" "));
          EscCursorLeft(1);
        }        
        break;
      case 10:
      case 13:
        break;
      default:
        // Print and save char
        Serial.print(c);
        strHLP[eos - 1] = c;
        break;
      }
    }
  }

  strHLP[eos - 1] = 0;
  EscColor(0);
  return 1;

}

long GetUserVal(long defVal, byte type){
  // type:  0 = int as it is
  //        1 = float (*1000)
  if (type){
    // Is scaled float
    IntToStr(defVal, 1, 3, ' ');
    strcpy(strHLP2, strHLP);
  }
  else{
    /* code */
    ltoa(defVal, strHLP2, 10);
  }
  if (GetUserString(strHLP2)){
    if (type){
      // Is scaled float
      strcpy(strHLP2, strHLP);
      defVal = StrToInt(strHLP2, 0);
    }
    else{
      /* code */
      defVal = atol(strHLP);
    }
  }
  return defVal;  
}

uint32_t GetUserTime(uint32_t timeIN){
  SerialDayTimeToStr(timeIN);
  GetUserString(strHLP2);
  return StrToTime(strHLP);
}

char GetUserKey(byte maxChar, byte noCnt){

  // noCnt = 0    No Numbers
  // noCnt = n     1-9
  
  byte timeOut = 60;
  char charIN = 0;
  char r = -1;         // TimeOut

  while (timeOut){

    if (DoTimer()){
      // A Second is over...
      timeOut--;
    }
    if (Serial.available()){
      charIN = Serial.read();
      timeOut = 0;
      r = charIN;
      if (charIN > 47 && charIN - 48 <= noCnt){
        // Valid Number selected
      }
      else if (charIN > 96 && charIN < maxChar + 1){
        // Valid Letter selected
      }
      else if (charIN == 13){
        // Enter - Exit - Back
        r = 0;
      }
      else{
        // Refresh
        r = -1;
      }
      if (r < 0){
        timeOut = 60;
      }
    } 
  }

  return r;

}

byte PrintLine(byte pos, byte start, byte len){
  EscLocate(start, pos++);
  for (int i = 0; i < len; i++){
    Serial.print(F("-"));
  }
  return pos;
}

byte PrintShortLine(byte pos, byte posX){
  pos = PrintLine(pos, posX, 3);
  return pos;
}

byte PrintBoldValue(long val, byte lz, byte dp, char lc){
  EscBold(1);
  byte r = IntToStr(val, lz, dp, lc);
  Serial.print(strHLP);
  EscBold(0);
  return r;
}

byte PrintMenuTop(char *strIN){

  EscCls();
  EscCursorVisible(1);
  EscInverse(1);
  EscLocate(1, 1);
  EscBold(1);
  Serial.print((char*)strIN);
  EscBold(0);
  EscInverse(0);
  return 2;

}

void PrintMenuEnd(byte pos){
  Serial.println(F("\n"));
  Serial.print(F("    Select key, or Enter(for return)..."));
}

void PrintSpacer(byte bold){
  EscBold(0);
  Serial.print(F(" | "));
  if (bold){
    EscBold(1);
  }
}

void PrintMenuKey(char key, char leadChar, char trailChar){
  EscBold(1);
  EscUnder(1);
  if (leadChar){
    Serial.print(leadChar);
  }
  Serial.print(key);
  Serial.print(F(")"));
  EscBold(0);
  EscUnder(0);
  Serial.print(F(": "));
  if (trailChar){
    Serial.print(trailChar);
  }
}

void PrintTimerLine1(byte timer, byte posX, byte posY){

  if (posX && posY){
    EscLocate(posX, posY);
  }
    
  // Name 16 chars
  EscBold(1);
  Serial.print(runningTimers[timer].name);
  EscBold(0);
  byte len = 16 - strlen(runningTimers[timer].name);
  while (len--){
    Serial.print(F(" "));
  }
  Serial.print(F("| "));

  // Type 11 chars
  if (runningTimers[timer].type.interrupted == 1){
    Serial.print(F("Inter-II"));
  }
  else if (runningTimers[timer].type.interval == 1){
    Serial.print(F("Interval"));
  }
  else if (runningTimers[timer].type.dayTimer == 1){
    Serial.print(F("  24h   "));
  }
  else{
    EscFaint(1);
    Serial.print(F("Disabled"));
  }
  EscFaint(0);
  PrintSpacer(0);
  
  // Check on automatic permanent & temporary state 9 chars
  if (runningTimers[timer].state.permOff){
    EscBold(1);
    EscColor(fgRed);
    Serial.print(F("perm.OFF"));
  }
  else if (runningTimers[timer].state.permOn){
    EscBold(1);
    EscColor(fgGreen);
    Serial.print(F("perm. ON"));
  }
  else if (runningTimers[timer].state.tempOff && myTime < runningTimers[timer].tempUntil){
    EscFaint(1);
    EscColor(fgRed);
    Serial.print(F("temp.OFF"));
  }
  else if (runningTimers[timer].state.tempOn && myTime < runningTimers[timer].tempUntil){
    EscFaint(1);
    EscColor(fgGreen);
    Serial.print(F("temp. ON"));
  }
  else if (runningTimers[timer].state.automatic){
    EscBold(1);
    Serial.print(F("  auto  "));
  }
  else{
    // Fully disabled
    EscFaint(1);
    Serial.print(F("disabled"));
  }
  EscBold(0);
  EscColor(0);
  PrintSpacer(0);


  // On / Off / N/A 3 chars
  if (runningTimers[timer].state.permOff){
    EscBold(1);
    EscColor(fgRed);
    Serial.print(F("OFF"));
  }
  else if (runningTimers[timer].state.permOn){
    EscBold(1);
    EscColor(fgGreen);
    Serial.print(F(" ON"));
  }
  else if (runningTimers[timer].state.tempOff && myTime < runningTimers[timer].tempUntil){
    EscFaint(1);
    EscColor(fgRed);
    Serial.print(F("OFF"));
  }
  else if (runningTimers[timer].state.tempOn && myTime < runningTimers[timer].tempUntil){
    EscFaint(1);
    EscColor(fgGreen);
    Serial.print(F(" ON"));
  }
  else if (runningTimers[timer].state.automatic){
    if (runningTimers[timer].state.lastVal){
      EscColor(fgGreen);
      Serial.print(F(" ON"));
    }
    else{
      EscColor(fgRed);
      Serial.print(F("OFF"));
    }
  }
  else{
    // Fully disabled
    EscFaint(1);
    Serial.print(F("N/A"));
  }
  EscBold(0);
  EscColor(0);
  PrintSpacer(0);

  uint32_t onDuration;
  uint32_t offDuration;
  uint32_t offset;
  uint32_t currentPos;
  uint32_t interval;
  if (runningTimers[timer].state.permOn || runningTimers[timer].state.permOff){
    // Permanent On/Off
    EscFaint(1);
    Serial.print(F("        N/A        "));
    PrintSpacer(0);
  }
  else if (runningTimers[timer].state.tempOn || runningTimers[timer].state.tempOff){
    // Temporary On/Off
    Serial.print(F("     "));
    PrintSerTime(runningTimers[timer].tempUntil,0);
    Serial.print(F("      "));
    PrintSpacer(0);
  }
  else if (runningTimers[timer].state.automatic){
    // Automatic
    if (runningTimers[timer].type.dayTimer){
      // 24h Day-Timer
      if (runningTimers[timer].offTime < runningTimers[timer].onTime){
        // Midnight Jump
        onDuration = (86400UL - runningTimers[timer].onTime) + runningTimers[timer].offTime;
      }
      else{
        onDuration = runningTimers[timer].offTime - runningTimers[timer].onTime;
      }
      offDuration = 86400UL - onDuration;
      offset = runningTimers[timer].onTime;
    }
    else{
      // Interval (both)
      onDuration = runningTimers[timer].onTime;
      offDuration = runningTimers[timer].offTime;
      offset = runningTimers[timer].offset;
    }
    
    currentPos = CurrentIntervalPos(myTime, onDuration, offDuration, offset);
    interval = onDuration + offDuration;
    if (currentPos < onDuration){
      // On since -> until
      EscColor(fgGreen);
      PrintSerTime(myTime - currentPos, 0);
      EscColor(39);
      Serial.print(F(" - "));
      EscColor(fgRed);
      PrintSerTime((myTime - currentPos) + onDuration, 0);
    }
    else{
      // Off since -> until
      EscColor(fgRed);
      PrintSerTime(myTime - currentPos + onDuration, 0);
      EscColor(39);
      Serial.print(F(" - "));
      EscColor(fgGreen);
      PrintSerTime((myTime - currentPos) + interval, 0);
    }
    EscColor(39);
    PrintSpacer(0);
  }
}

void PrintTimerLineShort(byte timer){
  // attach Properties list (up to 34 chars) on actual cursor-pos
  if (runningTimers[timer].type.interval || runningTimers[timer].type.interrupted || runningTimers[timer].type.dayTimer){
    PrintSerTime(runningTimers[timer].onTime, 0);
    Serial.print(F("|"));
    PrintSerTime(runningTimers[timer].offTime, 0);
    Serial.print(F("|"));
  }  
  if (runningTimers[timer].type.interrupted){
    PrintSerTime(runningTimers[timer].offset, 0);
    Serial.print(F("|"));
    PrintSerTime(runningTimers[timer].onTime2, 0);
    Serial.print(F("|"));
    PrintSerTime(runningTimers[timer].offTime2, 0);
  }
  else if (runningTimers[timer].type.interval){
    PrintSerTime(runningTimers[timer].offset, 0);
  }
  Serial.print(F(" |"));
}

void PrintTimerLineHeader(byte timer, byte posX, byte posY){
  if (posX && posY){
    EscLocate(posX, posY);
  }
  if (runningTimers[timer].type.interval || runningTimers[timer].type.interrupted || runningTimers[timer].type.dayTimer){
    // A timer is set
    EscBold(1);
    Serial.print(F("On-Time "));
    PrintSpacer(1);
    Serial.print(F("Off-Time"));
    EscSaveCursor();
    PrintLine(posY + 1, posX, 21);
    EscRestoreCursor();
    EscBold(0);
  }  
  if (runningTimers[timer].type.interrupted){
    PrintSpacer(1);
    Serial.print(F(" Offset "));
    PrintSpacer(1);
    Serial.print(F("OnTime-2"));
    PrintSpacer(1);
    Serial.print(F("OffTime2"));
    EscSaveCursor();
    PrintLine(posY + 1, posX + 21, 33);
    EscRestoreCursor();
    EscBold(0);
  }
  else if (runningTimers[timer].type.interval){
    PrintSpacer(1);
    Serial.print(F(" Offset "));
    EscSaveCursor();
    PrintLine(posY + 1, posX + 21, 11);
    EscRestoreCursor();
    EscBold(0);
  }
  PrintSpacer(0);
}

void PrintTimerLineLong(byte timer, byte posX, byte posY){
  // attach Properties list (up to 34 chars) on actual cursor-pos
  if (posX && posY){
    EscLocate(posX, posY);
  }
  if (runningTimers[timer].type.interval || runningTimers[timer].type.interrupted || runningTimers[timer].type.dayTimer){
    PrintSerTime(runningTimers[timer].onTime, 0);
    PrintSpacer(0);
    PrintSerTime(runningTimers[timer].offTime, 0);
    EscSaveCursor();
    PrintLine(posY + 1, posX, 21);
    EscRestoreCursor();
  }  
  if (runningTimers[timer].type.interrupted){
    PrintSpacer(0);
    PrintSerTime(runningTimers[timer].offset, 0);
    PrintSpacer(0);
    PrintSerTime(runningTimers[timer].onTime2, 0);
    PrintSpacer(0);
    PrintSerTime(runningTimers[timer].offTime2, 0);
    EscSaveCursor();
    PrintLine(posY + 1, posX + 21, 33);
    EscRestoreCursor();
  }
  else if (runningTimers[timer].type.interval){
    PrintSpacer(0);
    PrintSerTime(runningTimers[timer].offset, 0);
    EscSaveCursor();
    PrintLine(posY + 1, posX + 21, 11);
    EscRestoreCursor();
  }
  PrintSpacer(0);
}

void PrintDateTimeMenu(){

  char pos = PrintMenuTop((char*)"                             - Edit Date & Time -                               ") + 1;
  long r = 0;
  PrintErrorOK(0, -1 ,(char*)"Set Date & Time...");
Start:

  pos = 3;

  EscLocate(8, pos++);
  Serial.print(F("A):    Day: "));
  PrintBoldValue((long)((long)myDay * 1000), 4, 0, ' ');
  EscLocate(8, pos++);
  Serial.print(F("B):  Month: "));
  PrintBoldValue((long)((long)myMonth * 1000), 4, 0, ' ');
  EscLocate(8, pos++);
  Serial.print(F("C):   Year: "));
  PrintBoldValue((long)((long)myYear * 1000), 4, 0, ' ');
  EscLocate(8, pos++);
  Serial.print(F("D):   Hour: "));
  PrintBoldValue((long)((long)myHour * 1000), 4, 0, ' ');
  EscLocate(8, pos++);
  Serial.print(F("E): Minute: "));
  PrintBoldValue((long)((long)myMin * 1000), 4, 0, ' ');
  EscLocate(8, pos++);
  Serial.print(F("F): Second: "));
  PrintBoldValue((long)((long)mySec * 1000), 4, 0, ' ');

  PrintMenuEnd(pos + 1);
  Serial.print(F("        "));
  EscCursorLeft(8);

  pos = GetUserKey('f', -1);

  switch (pos){
  case -1:
    // TimeOut
  case 0:
    // EXIT
    pos = -1;
    break;
  case 'a':
    r = GetUserVal(myDay, 0);
    if (r > 0 && r <= GetDaysOfMonth(myMonth, myYear)){
      // Valid Day
      myDay = r;
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Illegal Day in this Month");
      pos = 0;
    }    
    break;
  case 'b':
    r = GetUserVal(myMonth, 0);
    if (r > 0 && r < 13 && myDay <= GetDaysOfMonth(r, myYear)){
      // Valid Month
      myMonth = r;
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Illegal Month with this Day/Year");
      pos = 0;
    }
    break;
  case 'c':
    r = GetUserVal(myYear, 0);
    if (r > 2022 && r < 2138 && myDay <= GetDaysOfMonth(myMonth, r)){
      // Valid Year
      myYear = r;
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Illegal Year with this Day/Month");
      pos = 0;
    }
    break;
  case 'd':
    r = GetUserVal(myHour, 0);
    if (r > -1 && r < 24){
      // Valid Hour
      myHour = r;
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Illegal Hour");
      pos = 0;
    }
    break;
  case 'e':
    r = GetUserVal(myMin, 0);
    if (r > -1 && r < 60){
      // Valid Minute
      myMin = r;
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Illegal Minute");
      pos = 0;
    }
    break;
  case 'f':
    r = GetUserVal(mySec, 0);
    if (r > -1 && r < 60){
      // Valid Second
      mySec = r;
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Illegal Second");
      pos = 0;
    }
    break;
  default:
    // BS
    pos = 0;
    break;
  }
  if (pos > 0){
    myTime = SerializeTime(myDay, myMonth, myYear, myHour, myMin, mySec);
    if (RTC_SetDateTime()){
      /* code */
      PrintErrorOK(1, -1, (char*)"RTC is set, too...");
    }
    else{
      PrintErrorOK(-1, -1, (char*)"Couldn't set RTC...");
    }
  }
  if (pos > -1){
    goto Start;
  }
  
}

void PrintEditMenu(byte timer){

  uint32_t tempTime = 900;

Start:

  char pos = PrintMenuTop((char*)"                                - Edit-Timer -                                  ") + 1;

  PrintLine(pos++, 4, 75);
  EscLocate(4, pos);
  PrintMenuKey('0', '(', ' ');
  PrintTimerLine1(timer, 10, pos++);
  PrintLine(pos++, 4, 75);
  EscLocate(9, pos);
  PrintSpacer(0);
  PrintTimerLineHeader(timer, 12, pos++);
  EscLocate(10, pos);
  EscBold(1);
  Serial.print(F("--"));
  pos++;
  EscLocate(9, pos);
  PrintSpacer(0);
  PrintTimerLineLong(timer, 12, pos++);
  EscLocate(10, pos);
  Serial.print(F("--"));
  pos++;
  EscLocate(9, pos);
  PrintSpacer(0);
  EscLocate(12, pos++);
  if (runningTimers[timer].type.interval || runningTimers[timer].type.interrupted || runningTimers[timer].type.dayTimer){
    // Timer is defined
    EscBold(1);
    Serial.print(F("  "));
    EscUnder(1);
    Serial.print(F("(1)"));
    EscUnder(0);
    Serial.print(F("   "));
    PrintSpacer(1);
    EscBold(1);
    Serial.print(F("  "));
    EscUnder(1);
    Serial.print(F("(2)"));
    EscUnder(0);
    Serial.print(F("   "));
  }  
  if (runningTimers[timer].type.interrupted){
    PrintSpacer(1);
    EscBold(1);
    Serial.print(F("  "));
    EscUnder(1);
    Serial.print(F("(3)"));
    EscUnder(0);
    Serial.print(F("   "));
    PrintSpacer(1);
    EscBold(1);
    Serial.print(F("  "));
    EscUnder(1);
    Serial.print(F("(4)"));
    EscUnder(0);
    Serial.print(F("   "));
    PrintSpacer(1);
    EscBold(1);
    Serial.print(F("  "));
    EscUnder(1);
    Serial.print(F("(5)"));
    EscUnder(0);
    Serial.print(F("   "));
  }
  else if (runningTimers[timer].type.interval){
    PrintSpacer(1);
    EscBold(1);
    Serial.print(F("  "));
    EscUnder(1);
    Serial.print(F("(3)"));
    EscUnder(0);
    Serial.print(F("   "));
  }
  PrintSpacer(0);

  PrintLine(pos++, 4, 75);

  EscLocate(4, pos++);
  PrintMenuKey('A', 0, 0);
  if (runningTimers[timer].type.interval && !runningTimers[timer].type.interrupted){
    EscBold(1);
  }
  Serial.print(F("Interval Timer     "));
  PrintMenuKey('B', 0, 0);
  if (runningTimers[timer].type.interrupted){
    EscBold(1);
  }  
  Serial.print(F("Double Interval Timer"));
  EscLocate(4, pos++);
  PrintMenuKey('C', 0, 0);
  if (runningTimers[timer].type.dayTimer){
    EscBold(1);
  }
  Serial.print(F("24h-Day Timer      "));
  PrintMenuKey('D', 0, 0);
  if (!runningTimers[timer].type.interval && !runningTimers[timer].type.interrupted && !runningTimers[timer].type.dayTimer){
    EscBold(1);
  }
  Serial.print(F("Disabled"));
  PrintShortLine(pos++, 4);
  EscLocate(4, pos++);
  
  PrintMenuKey('E',0,0);
  if (!runningTimers[timer].weekDays){
    EscBold(1);
  }
  else{
    EscFaint(1);
  }
  Serial.print(F("ALL  "));
  PrintMenuKey('F',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 1)){
    EscFaint(1);
  }
  Serial.print(F("Sun  "));
  PrintMenuKey('G',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 2)){
    EscFaint(1);
  }
  Serial.print(F("Mon  "));
  PrintMenuKey('H',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 3)){
    EscFaint(1);
  }
  Serial.print(F("TUE  "));
  PrintMenuKey('I',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 4)){
    EscFaint(1);
  }
  Serial.print(F("Wed  "));
  PrintMenuKey('J',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 5)){
    EscFaint(1);
  }
  Serial.print(F("Thu  "));
  PrintMenuKey('K',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 6)){
    EscFaint(1);
  }
  Serial.print(F("Fri  "));
  PrintMenuKey('L',0,0);
  if (!bitRead(runningTimers[timer].weekDays, 7)){
    EscFaint(1);
  }
  Serial.print(F("Sat  "));  
  //Serial.print(F("E): All  F): Sun  G): Mon  H): Tue  I): Wed  J): Thu  K): Fri  L): Sat"));
  PrintShortLine(pos++, 4);
  EscLocate(4, pos++);
  PrintMenuKey('M', 0, 0);
  if (runningTimers[timer].state.automatic){
    EscBold(1);
  }
  Serial.print(F("Automatic          "));
  PrintMenuKey('N', 0, 0);
  if (runningTimers[timer].state.permOff){
    EscBold(1);
  }
  Serial.print(F("Permanent OFF      "));
  PrintMenuKey('O', 0, 0);
  if (runningTimers[timer].state.permOn){
    EscBold(1);
  }
  Serial.print(F("Permanent ON"));
  PrintShortLine(pos++, 4);
  EscLocate(4, pos++);
  PrintMenuKey('P', 0, 0);
  if (runningTimers[timer].state.tempOff){
    EscBold(1);
  }
  Serial.print(F("Temporary OFF      "));
  PrintMenuKey('Q', 0, 0);
  if (runningTimers[timer].state.tempOn){
    EscBold(1);
  }
  Serial.print(F("Temporary ON       "));
  PrintMenuKey('R', 0, 0);
  Serial.print(F("SetTempTime: "));
  EscBold(1);
  PrintSerTime(tempTime, 0);
  EscBold(0);

  PrintLine(pos++, 4, 75);

  PrintMenuEnd(pos + 1);

  pos = GetUserKey('r', 5);

  switch (pos){
  case -1:
    // TimeOut
  case 0:
    // EXIT
    break;
  case '0':
    // Name
    GetUserString(runningTimers[timer].name);
    strcpy(runningTimers[timer].name, strHLP);
    break;
  case '1':
    // OnTime
    runningTimers[timer].onTime = GetUserTime(runningTimers[timer].onTime);
    break;
  case '2':
    // OffTime
    runningTimers[timer].offTime = GetUserTime(runningTimers[timer].offTime);
    break;
  case '3':
    // Offset
    runningTimers[timer].offset = GetUserTime(runningTimers[timer].offset);
    break;
  case '4':
    // OnTime2
    runningTimers[timer].onTime2 = GetUserTime(runningTimers[timer].onTime2);
    break;
  case '5':
    // OffTime2
    runningTimers[timer].offTime2 = GetUserTime(runningTimers[timer].offTime2);
    break;
  case 'a':
    // IntervalTimer
    runningTimers[timer].type.interval = 1;
    runningTimers[timer].type.interrupted = 0;
    runningTimers[timer].type.dayTimer = 0;
    break;
  case 'b':
    // Double IntervalTimer
    runningTimers[timer].type.interval = 1;
    runningTimers[timer].type.interrupted = 1;
    runningTimers[timer].type.dayTimer = 0;
    break;
  case 'c':
    // 24h Timer
    runningTimers[timer].type.interval = 0;
    runningTimers[timer].type.interrupted = 0;
    runningTimers[timer].type.dayTimer = 1;
    break;
  case 'd':
    // Disabled
    runningTimers[timer].type.interval = 0;
    runningTimers[timer].type.interrupted = 0;
    runningTimers[timer].type.dayTimer = 0;
    runningTimers[timer].state.automatic = 0;
    break;
  case 'e':
    // All Days
    runningTimers[timer].weekDays = 0;
    break;
  case 'f':
    // Sunday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 1, !getBit(runningTimers[timer].weekDays, 1));
    break;
  case 'g':
    // Monday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 2, !getBit(runningTimers[timer].weekDays, 2));
    break;
  case 'h':
    // Tuesday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 3, !getBit(runningTimers[timer].weekDays, 3));
    break;
  case 'i':
    // Wednesday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 4, !getBit(runningTimers[timer].weekDays, 4));
    break;
  case 'j':
    // Thursday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 5, !getBit(runningTimers[timer].weekDays, 5));
    break;
  case 'k':
    // Friday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 6, !getBit(runningTimers[timer].weekDays, 6));
    break;
  case 'l':
    // Saturday
    runningTimers[timer].weekDays = setBit(runningTimers[timer].weekDays, 7, !getBit(runningTimers[timer].weekDays, 7));
    break;
  case 'm':
    // Automatic
    runningTimers[timer].state.automatic = 1;
    runningTimers[timer].state.permOn = 0;
    runningTimers[timer].state.permOff = 0;
    runningTimers[timer].state.tempOn = 0;
    runningTimers[timer].state.tempOff = 0;
    runningTimers[timer].state.hasChanged = 1;
    break;
  case 'n':
    // perm. Off
    runningTimers[timer].state.automatic = 0;
    runningTimers[timer].state.permOn = 0;
    runningTimers[timer].state.permOff = 1;
    runningTimers[timer].state.tempOn = 0;
    runningTimers[timer].state.tempOff = 0;
    runningTimers[timer].state.hasChanged = 1;
    break;
  case 'o':
    // perm. On
    runningTimers[timer].state.automatic = 0;
    runningTimers[timer].state.permOn = 1;
    runningTimers[timer].state.permOff = 0;
    runningTimers[timer].state.tempOn = 0;
    runningTimers[timer].state.tempOff = 0;
    runningTimers[timer].state.hasChanged = 1;
    break;
  case 'p':
    // temp off
    runningTimers[timer].state.tempOff = 1;
    runningTimers[timer].state.tempOn = 0;
    runningTimers[timer].state.hasChanged = 1;
    runningTimers[timer].tempUntil = myTime + tempTime;
    pos = -1;
    break;
  case 'q':
    // temp on
    runningTimers[timer].state.tempOn = 1;
    runningTimers[timer].state.tempOff = 0;
    runningTimers[timer].state.hasChanged = 1;
    runningTimers[timer].tempUntil = myTime + tempTime;
    pos = -1;
    break;
  case 'r':
    // set temporary time
    tempTime = GetUserTime(tempTime);
    break;
  default:
    break;
  }
  if (pos > 0){
    if (pos != 'r'){
      EEPROM.put(timer * sizeof(TimerSTRUCT), runningTimers[timer]);
    }  
    goto Start;
  }  
}

void PrintLoopMenu(){

  byte r = 1;
  byte pos = PrintMenuTop((char*)"                                - QuickTimer 1.00 -                             ");
  EscCursorVisible(0);
  pos += 2;

  pos = PrintLine(pos, 5, 71);

  for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
    // Check state  
    if (runningTimers[i].state.permOff){   
    }
    else if (runningTimers[i].state.permOn){  
    }
    else if (runningTimers[i].state.tempOff && myTime < runningTimers[i].tempUntil){   
    }
    else if (runningTimers[i].state.tempOn && myTime < runningTimers[i].tempUntil){ 
    }
    else if (runningTimers[i].state.automatic){
    }
    else{
      // Fully disabled
      r = 0;
    }
    if (r){
      EscLocate(5, pos);
      PrintMenuKey((char)(65 + i), 0, ' ');
      PrintTimerLine1(i, 9, pos++);
    }
    r = 1;
  }
  
  PrintErrorOK(0, -1 ,(char*)"Loop started...");

  EscBold(1);
  pos = PrintLine(pos, 5, 71);
  EscBold(0);

}

void PrintMainMenu(){

Start:

  char pos = PrintMenuTop((char*)"                         - Main Menu QuickTimer 1.00 -                          ");
  
  for (int i = 0; i < RUNNING_TIMERS_CNT; i++){
    pos++;
    EscLocate(5, pos);
    PrintMenuKey((char)(65 + i), 0, ' ');
    PrintTimerLine1(i, 0, 0);
  }

  pos = PrintShortLine(pos + 1, 8);

  EscLocate(5, pos);
  PrintMenuKey('1', 0, 0);
  Serial.print(F("ReBoot"));
  EscLocate(18, pos);
  PrintMenuKey('5', 0, 0);
  Serial.print(F("Set Date & Time"));
  EscLocate(40, pos++);
  PrintMenuKey('6', 0, 0);
  Serial.print(F("Set Speed = "));
  EscBold(1);
  Serial.print(mySpeed);

  EscLocate(5, pos);
  PrintMenuKey('2', 0, 0);
  Serial.print(F("Boot4Terminal = "));
  if (myBoot){
    EscFaint(1);
    Serial.print(F("False"));
  }
  else{
    EscBold(1);
    Serial.print(F("True"));
  }
  EscBold(0);
  EscLocate(32, pos);
  PrintMenuKey('3', 0, 0);
  Serial.print(F("Boot4Slave = "));
  if (myBoot){
    EscBold(1);
    Serial.print(F("True"));
  }
  else{
    EscFaint(1);
    Serial.print(F("False"));
  }
  EscBold(0);
  EscLocate(58, pos++);
  PrintMenuKey('4', 0, 0);
  Serial.print(F("Address = "));
  PrintBoldValue((long)myAddress * 1000, 3, 0, '0');
  
  PrintMenuEnd(pos + 1);

  pos = GetUserKey(RUNNING_TIMERS_CNT - 1 + 97, 6);
  switch (pos){
  case -1:
    // TimeOut
  case 0:
    // EXIT
    break;
  case '1':
    // ReBoot
    break;
  case '2':
    // Boot for Terminal
    break;
  case '3':
    // Boot for Slave
    break;
  case '4':
    // Slave Address
    break;
  case '5':
    // Set Date & Time
    PrintDateTimeMenu();
    break;
  case '6':
    // Set Speed
    break;
  case '0':
  case '7':
  case '8':
  case '9':
    // NA
    break;
  default:
    // Timer
    if (pos > 96 && pos < 97 + RUNNING_TIMERS_CNT){
      PrintEditMenu(pos - 97);
    }
    break;
  }
  if (pos > 0){
    goto Start;
  }
  PrintLoopMenu();
}

