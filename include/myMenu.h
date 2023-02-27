
#include "myIIC.h"
#include "myTime.h"

byte myBoot = 0;    // 0 = Terminal  /  1 = Slave
uint32_t mySpeed = 9600;
byte mySolarized = 0;

byte IsSerialSpeedValid(uint32_t speed){
    switch (speed){
    case 1200:
    case 2400:
    case 4800:
    case 9600:
    case 14400:
    case 19200:
    case 28800:
    case 38400UL:
    case 57600UL:
    case 115200UL:
      // valid
      return 1;
      break;
    default:
      // illegal
      return 0;
      break;
    }
}

void myToRom(){
  EEPROM.put(1000, myBoot);
  EEPROM.put(1001, mySolarized);
  EEPROM.put(1002, myAddress);
  EEPROM.put(1003, mySpeed);
  // 1007 is next...
}

void myFromRom(){
  EEPROM.get(1000, myBoot);
  EEPROM.get(1001, mySolarized);
  EEPROM.get(1002, myAddress);
  EEPROM.get(1003, mySpeed);
  // 1007 is next...

  if (!IsSerialSpeedValid(mySpeed)){
    mySpeed = 9600;
  }
  if (!myAddress || myAddress > 254){
    myAddress = 123;
  }
  if (mySolarized){
    fgFaint = 92;
  }
  else{
    fgFaint = 90;
  }
  
  
  
}

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

uint32_t GetUserDate(uint32_t timeIN){
  SerialDateToStr(timeIN);
  GetUserString(strHLP2);
  return StrToDate(strHLP);
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
  if (pos && start){
    EscLocate(start, pos++);
  }
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

void PrintMenuNo(byte number, byte bold){
  EscBold(1);
  Serial.print(F(" "));
  EscUnder(1);
  Serial.print(F("("));
  Serial.print(number);
  Serial.print(F(")"));
  EscUnder(0);
  if (!bold){
    EscBold(0);
  }
}

void PrintTimerLine1(byte timer, byte posX, byte posY){


  if (posX && posY){
    EscLocate(posX, posY);
  }
    
  // Name 16 chars
  EscBold(1);
  Serial.print(runningTimer.name);
  EscBold(0);
  byte len = 16 - strlen(runningTimer.name);
  while (len--){
    Serial.print(F(" "));
  }
  Serial.print(F("| "));

  // Type 11 chars
  if (runningTimer.type.tripleI == 1){
    Serial.print(F("InterIII"));
  }
  else if (runningTimer.type.doubleI == 1){
    Serial.print(F("Inter-II"));
  }
  else if (runningTimer.type.interval == 1){
    Serial.print(F("Interval"));
  }
  else if (runningTimer.type.dayTimer == 1){
    Serial.print(F("  24h   "));
  }
  else{
    EscFaint(1);
    Serial.print(F("Disabled"));
  }
  EscFaint(0);
  PrintSpacer(0);
  
  // Check on automatic permanent & temporary state 9 chars
  if (runningTimer.state.permOff){
    EscBold(1);
    EscColor(fgRed);
    Serial.print(F("perm.OFF"));
  }
  else if (runningTimer.state.permOn){
    EscBold(1);
    EscColor(fgGreen);
    Serial.print(F("perm. ON"));
  }
  else if (runningTimer.state.tempOff && myTime < runningTimer.tempUntil){
    EscFaint(1);
    EscColor(fgRed);
    Serial.print(F("temp.OFF"));
  }
  else if (runningTimer.state.tempOn && myTime < runningTimer.tempUntil){
    EscFaint(1);
    EscColor(fgGreen);
    Serial.print(F("temp. ON"));
  }
  else if (runningTimer.state.automatic){
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
  if (runningTimer.state.permOff){
    EscBold(1);
    EscColor(fgRed);
    Serial.print(F("OFF"));
  }
  else if (runningTimer.state.permOn){
    EscBold(1);
    EscColor(fgGreen);
    Serial.print(F(" ON"));
  }
  else if (runningTimer.state.tempOff && myTime < runningTimer.tempUntil){
    EscFaint(1);
    EscColor(fgRed);
    Serial.print(F("OFF"));
  }
  else if (runningTimer.state.tempOn && myTime < runningTimer.tempUntil){
    EscFaint(1);
    EscColor(fgGreen);
    Serial.print(F(" ON"));
  }
  else if (runningTimer.state.automatic){
    if (runningTimer.state.lastVal){
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
  if (runningTimer.state.permOn || runningTimer.state.permOff){
    // Permanent On/Off
    EscFaint(1);
    Serial.print(F("        N/A        "));
    PrintSpacer(0);
  }
  else if (runningTimer.state.tempOn || runningTimer.state.tempOff){
    // Temporary On/Off
    Serial.print(F("     "));
    PrintSerTime(runningTimer.tempUntil,0);
    Serial.print(F("      "));
    PrintSpacer(0);
  }
  else if (runningTimer.state.automatic){
    // Automatic
    if (runningTimer.type.dayTimer){
      // 24h Day-Timer
      if (runningTimer.offTime[0] < runningTimer.onTime[0]){
        // Midnight Jump
        onDuration = (86400UL - runningTimer.onTime[0]) + runningTimer.offTime[0];
      }
      else{
        onDuration = runningTimer.offTime[0] - runningTimer.onTime[0];
      }
      offDuration = 86400UL - onDuration;
      offset = runningTimer.onTime[0];
    }
    else{
      // Interval (all)
      onDuration = runningTimer.onTime[0];
      offDuration = runningTimer.offTime[0];
      offset = runningTimer.offset[0];
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

byte PrintTimerTable(byte timer, byte posX, byte posY){
  
  byte r = 0;
  byte r2 = 3;

  if (posX && posY){
    // Top Left 
    EscLocate(posX, posY);
  }

  //           1         2         3         4         5         6
  // 1234567890123456789012345678901234567890123456789012345678901234567890
  // -------------------------------------------------------------------------------
  //            |     Main     |   whileON    |   whileOFF   |
  // ---------------------------------------------------------
  // |  OnTime: | 00:00:00 (1) | 00:00:00 (4) | 00:00:00 (7) |
  // ---------------------------------------------------------
  // | OffTime: | 00:00:00 (2) | 00:00:00 (5) | 00:00:00 (8) |
  // ---------------------------------------------------------
  // |  Offset: | 00:00:00 (3) | 00:00:00 (6) | 00:00:00 (9) |
  // ---------------------------------------------------------


  if (runningTimer.type.dayTimer){
    // DayTimer
    r = 1;
  }
  else if (runningTimer.type.interval){
    // Interval
    r = 2;
    if (runningTimer.type.doubleI || runningTimer.type.tripleI){
      // Double
      r++;
      if (runningTimer.type.tripleI){
        // Triple
        r++;
      }
    }
  }
  
  // Top-Line
  EscSaveCursor();
  if (r > 0){
    // 24h & Interval
    EscCursorRight(10);
    PrintSpacer(1);
    Serial.print(F("    Main    "));
    PrintSpacer(1);
    r2 = 6;
  }
  if (r > 2){
    // double & triple
    Serial.print(F("  while ON  "));
    PrintSpacer(1);
  }
  if (r > 3){
    // triple
    Serial.print(F(" while OFF  "));
    PrintSpacer(1);
  }
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  EscBold(1);
  PrintLine(0, 0, 57);
  EscBold(0);
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  Serial.print(F("|"));
  EscBold(1);

  // OnTime-Line
  if (r > 0){
    // 24h & Interval
    Serial.print(F("  OnTime"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    PrintSerTime(runningTimer.onTime[0], 0);
    PrintMenuNo(1, 0);
    PrintSpacer(0);
  }
  if (r > 2){
    // double & triple
    PrintSerTime(runningTimer.onTime[1], 0);
    PrintMenuNo(4, 0);
    PrintSpacer(0);
  }
  if (r > 3){
    // triple
    PrintSerTime(runningTimer.onTime[2], 0);
    PrintMenuNo(7, 0);
    PrintSpacer(0);
  }
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  PrintLine(0, 0, 57);
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  Serial.print(F("|"));
  EscBold(1);

  // OffTime-Line
  if (r > 0){
    // 24h & Interval
    Serial.print(F(" OffTime"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    PrintSerTime(runningTimer.offTime[0], 0);
    PrintMenuNo(2, 0);
    PrintSpacer(0);
  }
  if (r > 2){
    // double & triple
    PrintSerTime(runningTimer.offTime[1], 0);
    PrintMenuNo(5, 0);
    PrintSpacer(0);
  }
  if (r > 3){
    // triple
    PrintSerTime(runningTimer.offTime[2], 0);
    PrintMenuNo(8, 0);
    PrintSpacer(0);
  }
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  PrintLine(0, 0, 57);
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  Serial.print(F("|"));
  EscBold(1);

  // Offset-Line
  if (r > 1){
    // Interval
    Serial.print(F("  OffSet"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    PrintSerTime(runningTimer.offset[0], 0);
    PrintMenuNo(3, 0);
    PrintSpacer(0);
    r2++;
  }
  if (r > 2){
    // double & triple
    PrintSerTime(runningTimer.offset[1], 0);
    PrintMenuNo(6, 0);
    PrintSpacer(0);
  }
  if (r > 3){
    // triple
    PrintSerTime(runningTimer.offset[2], 0);
    PrintMenuNo(9, 0);
    PrintSpacer(0);
  }
  if (r > 1){
    EscRestoreCursor();
    EscCursorDown(1);
    PrintLine(0, 0, 57);
    r2++;
  }
  return r2;
}

void PrintEditMenu(byte timer){

  uint32_t tempTime = 900;
  byte isChanged = 0;

  TimerFromRomRam(timer, 1);

Start:

  char pos = PrintMenuTop((char*)"                                - Edit-Timer -                                  ") + 1;
  pos--;
  PrintLine(pos++, 4, 75);
  EscLocate(4, pos);
  PrintMenuKey('0', '(', ' ');
  PrintTimerLine1(timer, 10, pos++);
  PrintLine(pos++, 4, 75);

  pos = pos + PrintTimerTable(timer, 11, pos);

  EscLocate(4, pos++);
  PrintMenuKey('A', 0, 0);
  if (runningTimer.type.interval && !runningTimer.type.doubleI && !runningTimer.type.tripleI){
    EscBold(1);
  }
  Serial.print(F("Interval Timer     "));
  
  PrintMenuKey('B', 0, 0);
  if (runningTimer.type.doubleI){
    EscBold(1);
  }  
  Serial.print(F("DoubleI Timer     "));

  PrintMenuKey('C', 0, 0);
  if (runningTimer.type.tripleI){
    EscBold(1);
  }  
  Serial.print(F("TripleI Timer"));
  
  EscLocate(4, pos++);
  PrintMenuKey('D', 0, 0);
  if (runningTimer.type.dayTimer){
    EscBold(1);
  }
  Serial.print(F("24h-Day Timer      "));

  PrintMenuKey('E', 0, 0);
  if (runningTimer.type.invert){
    EscBold(1);
  }
  Serial.print(F("Inverted Port     "));

  PrintMenuKey('F', 0, 0);
  if (!runningTimer.type.interval && !runningTimer.type.dayTimer){
    EscBold(1);
  }
  Serial.print(F("Disabled"));
  PrintShortLine(pos++, 4);
  
  EscLocate(4, pos++);
  PrintMenuKey('G',0,0);
  if (!runningTimer.weekDays){
    EscBold(1);
  }
  else{
    EscFaint(1);
  }
  Serial.print(F("ALL  "));
  PrintMenuKey('H',0,0);
  if (!bitRead(runningTimer.weekDays, 1)){
    EscFaint(1);
  }
  Serial.print(F("Sun  "));
  PrintMenuKey('I',0,0);
  if (!bitRead(runningTimer.weekDays, 2)){
    EscFaint(1);
  }
  Serial.print(F("Mon  "));
  PrintMenuKey('J',0,0);
  if (!bitRead(runningTimer.weekDays, 3)){
    EscFaint(1);
  }
  Serial.print(F("TUE  "));
  PrintMenuKey('K',0,0);
  if (!bitRead(runningTimer.weekDays, 4)){
    EscFaint(1);
  }
  Serial.print(F("Wed  "));
  PrintMenuKey('L',0,0);
  if (!bitRead(runningTimer.weekDays, 5)){
    EscFaint(1);
  }
  Serial.print(F("Thu  "));
  PrintMenuKey('M',0,0);
  if (!bitRead(runningTimer.weekDays, 6)){
    EscFaint(1);
  }
  Serial.print(F("Fri  "));
  PrintMenuKey('N',0,0);
  if (!bitRead(runningTimer.weekDays, 7)){
    EscFaint(1);
  }
  Serial.print(F("Sat  "));  

  PrintShortLine(pos++, 4);
  EscLocate(4, pos++);
  PrintMenuKey('O', 0, 0);
  if (runningTimer.state.automatic){
    EscBold(1);
  }
  Serial.print(F("Automatic          "));
  PrintMenuKey('P', 0, 0);
  if (runningTimer.state.permOff){
    EscBold(1);
  }
  Serial.print(F("Permanent OFF      "));
  PrintMenuKey('Q', 0, 0);
  if (runningTimer.state.permOn){
    EscBold(1);
  }
  Serial.print(F("Permanent ON"));
  PrintShortLine(pos++, 4);
  EscLocate(4, pos++);
  PrintMenuKey('R', 0, 0);
  if (runningTimer.state.tempOff){
    EscBold(1);
  }
  Serial.print(F("Temporary OFF      "));
  PrintMenuKey('S', 0, 0);
  if (runningTimer.state.tempOn){
    EscBold(1);
  }
  Serial.print(F("Temporary ON       "));
  
  PrintMenuKey('T', 0, 0);
  Serial.print(F("SetTempTime: "));
  EscBold(1);
  PrintSerTime(tempTime, 0);
  EscBold(0);

  PrintLine(pos++, 4, 75);

  PrintMenuEnd(pos + 1);

  pos = GetUserKey('t', 9);

  switch (pos){
  case -1:
    // TimeOut
  case 0:
    // EXIT
    break;
  case '0':
    // Name
    GetUserString(runningTimer.name);
    strcpy(runningTimer.name, strHLP);
    break;
  case '1':
    // OnTime
    runningTimer.onTime[0] = GetUserTime(runningTimer.onTime[0]);
    break;
  case '2':
    // OffTime
    runningTimer.offTime[0] = GetUserTime(runningTimer.offTime[0]);
    break;
  case '3':
    // Offset
    runningTimer.offset[0] = GetUserTime(runningTimer.offset[0]);
    break;
  case '4':
    // OnTime
    runningTimer.onTime[1] = GetUserTime(runningTimer.onTime[1]);
    break;
  case '5':
    // OffTime
    runningTimer.offTime[1] = GetUserTime(runningTimer.offTime[1]);
    break;
  case '6':
    // Offset
    runningTimer.offset[1] = GetUserTime(runningTimer.offset[1]);
    break;
  case '7':
    // OnTime
    runningTimer.onTime[2] = GetUserTime(runningTimer.onTime[2]);
    break;
  case '8':
    // OffTime
    runningTimer.offTime[2] = GetUserTime(runningTimer.offTime[2]);
    break;
  case '9':
    // Offset
    runningTimer.offset[2] = GetUserTime(runningTimer.offset[2]);
    break;
  case 'a':
    // IntervalTimer
    runningTimer.type.interval = 1;
    runningTimer.type.doubleI = 0;
    runningTimer.type.tripleI = 0;
    runningTimer.type.dayTimer = 0;
    break;
  case 'b':
    // Double IntervalTimer
    runningTimer.type.interval = 1;
    runningTimer.type.doubleI = 1;
    runningTimer.type.tripleI = 0;
    runningTimer.type.dayTimer = 0;
    break;
  case 'c':
    // Triple IntervalTimer
    runningTimer.type.interval = 1;
    runningTimer.type.doubleI = 0;
    runningTimer.type.tripleI = 1;
    runningTimer.type.dayTimer = 0;
    break;
  case 'd':
    // 24h Timer
    runningTimer.type.interval = 0;
    runningTimer.type.doubleI = 0;
    runningTimer.type.tripleI = 0;
    runningTimer.type.dayTimer = 1;
    break;
  case 'e':
    // Inverted
    runningTimer.type.invert = !runningTimer.type.invert;
  case 'f':
    // Disabled
    runningTimer.type.interval = 0;
    runningTimer.type.doubleI = 0;
    runningTimer.type.tripleI = 0;
    runningTimer.type.dayTimer = 0;
    runningTimer.state.automatic = 0;
    break;
  case 'g':
    // All Days
    runningTimer.weekDays = 0;
    break;
  case 'h':
    // Sunday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 1, !getBit(runningTimer.weekDays, 1));
    break;
  case 'i':
    // Monday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 2, !getBit(runningTimer.weekDays, 2));
    break;
  case 'j':
    // Tuesday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 3, !getBit(runningTimer.weekDays, 3));
    break;
  case 'k':
    // Wednesday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 4, !getBit(runningTimer.weekDays, 4));
    break;
  case 'l':
    // Thursday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 5, !getBit(runningTimer.weekDays, 5));
    break;
  case 'm':
    // Friday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 6, !getBit(runningTimer.weekDays, 6));
    break;
  case 'n':
    // Saturday
    runningTimer.weekDays = setBit(runningTimer.weekDays, 7, !getBit(runningTimer.weekDays, 7));
    break;
  case 'o':
    // Automatic
    runningTimer.state.automatic = 1;
    runningTimer.state.permOn = 0;
    runningTimer.state.permOff = 0;
    runningTimer.state.tempOn = 0;
    runningTimer.state.tempOff = 0;
    runningTimer.state.hasChanged = 1;
    break;
  case 'p':
    // perm. Off
    runningTimer.state.automatic = 0;
    runningTimer.state.permOn = 0;
    runningTimer.state.permOff = 1;
    runningTimer.state.tempOn = 0;
    runningTimer.state.tempOff = 0;
    runningTimer.state.hasChanged = 1;
    break;
  case 'q':
    // perm. On
    runningTimer.state.automatic = 0;
    runningTimer.state.permOn = 1;
    runningTimer.state.permOff = 0;
    runningTimer.state.tempOn = 0;
    runningTimer.state.tempOff = 0;
    runningTimer.state.hasChanged = 1;
    break;
  case 'r':
    // temp off
    runningTimer.state.tempOff = 1;
    runningTimer.state.tempOn = 0;
    runningTimer.state.hasChanged = 1;
    runningTimer.tempUntil = myTime + tempTime;
    pos = -1;
    break;
  case 's':
    // temp on
    runningTimer.state.tempOn = 1;
    runningTimer.state.tempOff = 0;
    runningTimer.state.hasChanged = 1;
    runningTimer.tempUntil = myTime + tempTime;
    pos = -1;
    break;
  case 't':
    // set temporary time
    tempTime = GetUserTime(tempTime);
    break;
  default:
    break;
  }
  if (pos > 0){
    if (pos != 'r'){
      isChanged = 1;
    }  
    goto Start;
  }  
  if (isChanged){
    TimerToRomRam(timer, 1);
  }
}

void PrintLoopMenu(){

  byte r = 1;
  byte pos = PrintMenuTop((char*)"                                - QuickTimer 1.01 -                             ");
  EscCursorVisible(0);
  pos += 2;

  pos = PrintLine(pos, 5, 71);

  for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
    // Check state  
    if (runningState[i].state.permOff){   
    }
    else if (runningState[i].state.permOn){  
    }
    else if (runningState[i].state.tempOff && myTime < runningState[i].tempUntil){   
    }
    else if (runningState[i].state.tempOn && myTime < runningState[i].tempUntil){ 
    }
    else if (runningState[i].state.automatic){
    }
    else{
      // Fully disabled
      r = 0;
    }
    if (r){
      EscLocate(5, pos);
      PrintMenuKey((char)(65 + i), 0, ' ');
      TimerFromRomRam(i, 0);
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

  char pos = PrintMenuTop((char*)"                         - Main Menu QuickTimer 1.01 -                          ");
  
  uint32_t hlpDate = 0;
  uint32_t hlpTime = 0;
  
  for (int i = 0; i < RUNNING_TIMERS_CNT; i++){
    pos++;
    EscLocate(5, pos);
    PrintMenuKey((char)(65 + i), 0, ' ');
    TimerFromRomRam(i, 1);
    PrintTimerLine1(i, 0, 0);
  }

  pos = PrintShortLine(pos + 1, 8);

  EscLocate(5, pos);
  PrintMenuKey('1', 0, 0);
  Serial.print(F("ReBoot"));
  EscLocate(18, pos);
  PrintMenuKey('2', 0, 0);
  Serial.print(F("Date"));
  EscLocate(29, pos);
  PrintMenuKey('3', 0, 0);
  Serial.print(F("Time"));
  EscLocate(40, pos);
  PrintMenuKey('4', 0, 0);
  Serial.print(F("Address = "));
  PrintBoldValue((long)myAddress * 1000, 3, 0, '0');
  EscLocate(60, pos++);
  PrintMenuKey('5', 0, 0);
  Serial.print(F("Speed = "));
  EscBold(1);
  Serial.print(mySpeed);
  
  EscLocate(5, pos);
  PrintMenuKey('6', 0, 0);
  Serial.print(F("Boot4Terminal = "));
  if (myBoot){
    EscFaint(1);
    Serial.print(F("False"));
    EscLocate(34, pos);
  }
  else{
    EscBold(1);
    Serial.print(F("True"));
    EscLocate(33, pos);
  }
  EscBold(0);
  PrintMenuKey('7', 0, 0);
  Serial.print(F("Boot4Slave = "));
  if (myBoot){
    EscBold(1);
    Serial.print(F("True"));
    EscLocate(58, pos++);
  }
  else{
    EscFaint(1);
    Serial.print(F("False"));
    EscLocate(59, pos++);
  }
  EscBold(0);
  PrintMenuKey('8', 0, 0);
  if (mySolarized){
    EscBold(1);
  }
  else{
    EscFaint(1);
  }
  Serial.print(F("Sol.Color"));
  EscBold(0);

  PrintMenuEnd(pos + 1);

  pos = GetUserKey(RUNNING_TIMERS_CNT - 1 + 97, 8);
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
    // Set Date
    hlpDate = SerializeTime(1, 1, 2023, myHour, myMin, mySec);    // Time of now
    DeSerializeTime(hlpDate + GetUserDate(myTime), &myDay, &myMonth, &myYear, &myHour, &myMin, &mySec);
    RTC_SetDateTime();
    myTime = SerializeTime(myDay, myMonth, myYear, myHour, myMin, mySec);
    break;
  case '3':
    // Set Time
    hlpTime = SerializeTime(myDay, myMonth, myYear, 0, 0, 0);    // Midnight of today
    DeSerializeTime(hlpTime + GetUserTime(myTime), &myDay, &myMonth, &myYear, &myHour, &myMin, &mySec);
    RTC_SetDateTime();
    myTime = SerializeTime(myDay, myMonth, myYear, myHour, myMin, mySec);
    break;
  case '4':
    // Set Address
    myAddress = GetUserVal(myAddress, 0);
    if (!myAddress || myAddress > 254){
      // illegal address - reload from eeprom
      myFromRom();
    }
    else{
      // save to eeprom...
      myToRom();
    }
    break;
  case '5':
    // Set Speed
    mySpeed = GetUserVal(mySpeed, 0);
    if (IsSerialSpeedValid(mySpeed)){ 
      // valid - save to eeprom
      myToRom();
    }
    else{
      // illegal - reload from eeprom
      myFromRom();
    }
    break;    
  case '6':
    // Boot Terminal
    break;
  case '7':
    // Boot Slave
    break;
  case '8':
    // Use Solarized Color-Hack
    mySolarized = !mySolarized;
    if (mySolarized){
      fgFaint = 92;
    }
    else{
      fgFaint = 90;
    }
    // Save to eeprom
    myToRom();
    break;
  case '0':
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

