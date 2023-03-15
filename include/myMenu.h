
#include "myIIC.h"
#include "myTime.h"

#define SMALL_GetUserVal 1


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

void PrintCharsCnt(char charToPrint, byte cnt){
  while (cnt--){
    Serial.print(charToPrint);
  }
}
#define PrintSpaces(cnt) PrintCharsCnt(' ', cnt)

#if SMALL_GetUserVal
#else
  long StrToInt(char *strIN, byte next){

      // "1.234,4.321" ==> 1234 (1st call with next = 0)
      // "1.234,4.321" ==> 4321 (next call with next = 1)

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
          // Floating point number

          char afterDotChars[] = "000";
          char *eofAfterDot = NULL;

          if (nextVal == NULL){
            // Next val doesn't exist
            eofAfterDot = strchr(actVal, '\0');
          }
          else{
            eofAfterDot = nextVal;
            nextVal++;
          }
          
          // count of digits after dot
          r = (long)(nextVal - dot) - 1;

          if (r > 3){
            // too much. Max precision is 1/1000
            r = 3;
          }
          memcpy(afterDotChars, dot + 1, r);

          afterDot = atol(afterDotChars);
      }
      else{
        // Integer number
        if (nextVal){
            // Valid next number - pointer on 1st char
            nextVal++;
        }
      }
      
      if (preDot >= 0){
          r = preDot + afterDot;
      }
      else{
          r = preDot - afterDot;
      }

      return r;
  }
#endif

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
  byte eos = strlen(strIN);   // Position of EndOfString
  byte pos = 0;               // Position of cursor
  
  byte escCnt = 0;            // hlp to interpret esc-sequences
  char escErr = 0;            // hlp to interpret esc-sequences
  byte escCmd = 0;

  EscBold(1);
  Serial.print(F(">> "));
  EscBold(0);
  EscColor(fgCyan);

  if (eos){
    pos = eos;
    Serial.print(strIN);
    strcpy(strHLP, strIN);
  }
  
  strHLP[eos] = 0;

  while (c != 13){

    if (DoTimer()){
      timeOut--;
      if (!timeOut){
        strcpy(strHLP, strIN);
        EscColor(0);
        return 0;
      }
    }

    if (Serial.available()){
      
      timeOut = 60;

      c = Serial.read();

      switch (c){
      case 27:
        // Take all other chars from SerialBuffer and check on cursor movement and unsupported ESC-Sequences
        // Single ESC is a user-esc
        escErr = 0;
        escCnt = 0;
        escCmd = 0;
        delay(12);
        while (Serial.available()){
          escCnt++;
          c = Serial.read();
          delay(12);
          switch (escCnt){
          case 1:
            if (c != 91){
              // unsupported
              escErr = 1;
            }
            break;
          case 2:
            switch (c){
            case 51:
              // del
            case 67:
              // right
            case 68:
              // left
              escCmd = c;
              break;
            default:
              // unsupported
              escErr = 1;
              break;
            }
            break;
          case 3:
            switch (c){
            case 126:
              switch (escCmd){
              case 51:
                // del
                break;
              default:
                escErr = 1;
                break;
              }
              break;
            default:
              escErr = 1;
              break;
            }
            break;
          default:
            // unsupported
            escErr = 1;
            break;
          }
        }
        if (!escCnt){
          // Single User ESC
          // Restore text and return
          strcpy(strHLP, strIN);
          eos = strlen(strHLP);
          c = 13;
        }
        else{
          if (!escErr){
            // Known Sequence
            switch (escCmd){
            case 67:
              // right
              if (pos < eos){
                pos++;
                EscCursorRight(1);
              }
              break;
            case 68:
              // left
              if (pos){
                pos--;
                EscCursorLeft(1);
              }
              break;
            case 51:
              // del
              if (pos < eos){
                memmove(&strHLP[pos], &strHLP[pos + 1], eos - pos);
                Serial.print(&strHLP[pos]);
                Serial.print(F(" "));
                EscCursorLeft(eos - pos);
                eos--;
              }
              break;
            }
          }
        }
        break;
      case 8:
      case 127:
        // Back
        if (pos && (pos == eos)){
          // Cursor is at the end
          eos--;
          pos--;
          EscCursorLeft(1);
          Serial.print(F(" "));
          EscCursorLeft(1);
          strHLP[eos] = 0;
        }
        else if (pos && (pos < eos)){
          // Cursor is 'somewhere' - shift chars 1 to left
          pos--;
          memmove(&strHLP[pos], &strHLP[pos + 1], eos - pos + 2);
          EscCursorLeft(1);
          Serial.print(&strHLP[pos]);
          Serial.print(F(" "));
          EscCursorLeft(eos - pos);
          eos--;
        }                
        break;
      case 10:
      case 13:
        break;
      default:
        // Print and save char
        if (c > 31 && c < 255){
          // Valid char
          if (pos == IIC_HLP_LEN){
            // MaxLen reached
            EscCursorLeft(1);
            pos--;
            eos--;
          }
          else if (pos < eos){
            // Cursor is 'somewhere' - shift chars 1 to right
            memmove(&strHLP[pos + 1], &strHLP[pos], eos - pos + 1);
            EscCursorRight(1);
            Serial.print(&strHLP[pos + 1]);
            EscCursorLeft(eos - pos + 1);
          }
          else{
            // pos & eos at the 1st or last position of string
            strHLP[pos + 1] = 0;
          }        
          Serial.print(c);
          strHLP[pos] = c;
          pos++;
          eos++;
        }        
        break;
      }
    }
  }

  strHLP[eos] = 0;
  EscColor(0);
  return 1;

}

#if SMALL_GetUserVal
  long GetUserVal(long defVal){
#else
  long GetUserVal(long defVal, byte type){
#endif
  // type:  0 = int as it is
  //        1 = float (*1000)
  
  // Set strHLP2 to representation of defVal
  #if SMALL_GetUserVal
    ltoa(defVal, strHLP2, 10);
    if (GetUserString(strHLP2)){
      defVal = atol(strHLP);
    }
  #else
    if (type){
      // Is scaled float
      IntToStr(defVal, 1, 3, ' ');
      strcpy(strHLP2, strHLP);
    }
    else{
      // Integer as it is
      ltoa(defVal, strHLP2, 10);
    }

    if (GetUserString(strHLP2)){
      if (type){
        // Is scaled float
        strcpy(strHLP2, strHLP);
        defVal = StrToInt(strHLP2, 0);
      }
      else{
        // Integer as it is
        defVal = atol(strHLP);
      }
    }
  #endif
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
  PrintCharsCnt('-', len);
  
  //for (int i = 0; i < len; i++){
    //Serial.print(F("-"));
  //}
  return pos;
}
#define PrintShortLine(pos, posX) PrintLine(pos, posX, 3)

byte PrintBoldValue(long val, byte lz, byte dp, char lc){
  EscBold(1);
  byte r = IntToStr(val, lz, dp, lc);
  Serial.print(strHLP);
  EscBold(0);
  return r;
}

byte PrintMenuTop(char *strIN){

  byte spaces = 80 - strlen(strIN);
  byte frontSpaces = spaces / 2;
  spaces -= frontSpaces;

  EscCls();
  EscCursorVisible(1);
  EscInverse(1);
  EscLocate(1, 1);
  EscBold(1);
  PrintSpaces(frontSpaces);
  Serial.print(strIN);
  PrintSpaces(spaces);
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

void PrintMenuKey(char key, byte space, char leadChar, char trailChar, byte colon, byte bold, byte faint){
  // "space" is a leading space - (as very 1st print)
  // "key" is all time bold and underlined followed by a ')'
  // "leadChar" (bold and underlined, too) is an option
  // "colon" ': ' (not bold, nor underlined) is an option
  // "bold" sets/keeps bold on exit

  if (space){
    Serial.print(F(" "));
  }
  EscBold(1);
  EscUnder(1);
  if (leadChar){
    Serial.print(leadChar);
  }
  Serial.print(key);
  Serial.print(F(")"));
  EscBold(0);
  EscUnder(0);
  if (colon){
    Serial.print(F(": "));
  }
  if (trailChar){
    Serial.print(trailChar);
  }
  if (bold){
    EscBold(1);
  }
  else if (faint){
    EscFaint(1);
  }
  
}
void PrintMenuKeyStd(char key){
  PrintMenuKey(key, 0, 0, 0, 1, 0, 0);
}
void PrintMenuNo(char number){
  PrintMenuKey(number, 1, '(', 0, 0, 0, 0);
}

void PrintTimerLine1(byte timer, byte posX, byte posY, byte name, byte type){

  // "timer" - the timer-id
  // "posX & posY" - if set, set cursor position
  // "name" - if the 'static' name is printed
  // "type" - 0 = type-area untouched
  //        - 1 = timer-type
  //        - 2 = last port-set-time

  if (posX && posY){
    EscLocate(posX, posY);
  }
    
  // Name 16 chars
  if (name){
    EscBold(1);
    Serial.print(runningTimer.name);
    EscBold(0);
    byte len = 16 - strlen(runningTimer.name);
    while (len--){
      Serial.print(F(" "));
    }
  }
  else{
    EscCursorRight(16);
  }  
  Serial.print(F("| "));

  // Type 11 chars
  if (type == 1){
    // Print Timer-Type
    if (runningTimer.type.interval){
      if (runningTimer.type.whileON && runningTimer.type.whileOFF){
        // whileOn and whileOff
        Serial.print(F("IntOnOff"));
      }
      else if (runningTimer.type.whileON){
        // whileOn
        Serial.print(F("Inter-ON"));
      }
      else if (runningTimer.type.whileOFF){
        // whileOff
        Serial.print(F("InterOFF"));
      }
      if (!runningTimer.type.whileON && !runningTimer.type.whileOFF){
        // just Interval
        Serial.print(F("Interval"));
      } 
    }    
    else if (runningTimer.type.dayTimer == 1){
      Serial.print(F("  24h   "));
    }
    else{
      EscFaint(1);
      Serial.print(F("Disabled"));
    }
  }
  else if (type == 2){
    // Print Time
    PrintSerTime(myTime, 0);
  }
  else{
    // Print Nothing
    EscCursorRight(8);
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
  uint32_t timeToUse;

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
      timeToUse = myTime;
    }
    else{
      // Interval (all)
      onDuration = runningTimer.onTime[0];
      offDuration = runningTimer.offTime[0];
      offset = runningTimer.offset[0];
      timeToUse = myTime;

      currentPos = CurrentIntervalPos(myTime, onDuration, offDuration, offset);

      if (currentPos < onDuration){
        // 1st Interval is ON
        if (runningTimer.type.whileON){
          timeToUse = currentPos;
          onDuration = runningTimer.onTime[1];
          offDuration = runningTimer.offTime[1];
          offset = runningTimer.offset[1];
        }
      }
      else{
      // 1st Interval is OFF
      if (runningTimer.type.whileOFF){
          timeToUse = currentPos - runningTimer.onTime[0];
          onDuration = runningTimer.onTime[2];
          offDuration = runningTimer.offTime[2];
          offset = runningTimer.offset[2];
        }    
      }
    }
    
    currentPos = CurrentIntervalPos(timeToUse, onDuration, offDuration, offset);
    interval = onDuration + offDuration;
    if (currentPos < onDuration){
      // On since -> until
      EscColor(fgGreen);
      PrintSerTime(myTime - currentPos, 0);
      EscColor(39);
      Serial.print(F(" - "));
      EscColor(fgRed);
      timeToUse = (myTime - currentPos) + onDuration;
    }
    else{
      // Off since -> until
      EscColor(fgRed);
      PrintSerTime(myTime - currentPos + onDuration, 0);
      EscColor(39);
      Serial.print(F(" - "));
      EscColor(fgGreen);
      timeToUse = (myTime - currentPos) + interval + offset;
    }
    PrintSerTime(timeToUse, 0);
    EscColor(39);
    PrintSpacer(0);
  }
}

void PrintTimerUnderLine(byte bold){
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  EscBold(bold);
  PrintLine(0, 0, 57);
  EscBold(0);
  EscRestoreCursor();
  EscCursorDown(1);
  EscSaveCursor();
  Serial.print(F("|"));
  EscBold(1);
}

byte PrintTimerTable(byte timer, byte posX, byte posY){
  
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

  
  // Top-Line
  EscSaveCursor();
  if (runningTimer.type.dayTimer || runningTimer.type.interval){
    // 24h & Interval
    EscCursorRight(10);
    PrintSpacer(1);
    Serial.print(F("    Main    "));
    PrintSpacer(1);
    r2 = 6;
    if (runningTimer.type.whileON){
      // whileON
      Serial.print(F("  while ON  "));
      PrintSpacer(1);
    }
    else if(runningTimer.type.whileOFF){
      // whileOFF exist...
      Serial.print(F("    N/A     "));
      PrintSpacer(1);
    }
    if (runningTimer.type.whileOFF){
      // whileOFF
      Serial.print(F(" while OFF  "));
      PrintSpacer(1);
    }
    PrintTimerUnderLine(1);
  }

  // OnTime-Line
  if (runningTimer.type.dayTimer || runningTimer.type.interval){
    // 24h & Interval
    Serial.print(F("  OnTime"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    PrintSerTime(runningTimer.onTime[0], 0);
    PrintMenuNo('1');
    PrintSpacer(0);
    if (runningTimer.type.whileON){
      // whileON
      PrintSerTime(runningTimer.onTime[1], 0);
      PrintMenuNo('4');
      PrintSpacer(0);
    }
    else if (runningTimer.type.whileOFF){
      // whileOFF exist
      Serial.print(F("   NA    ( )"));
      PrintSpacer(0);
    }  
    if (runningTimer.type.whileOFF){
      // whileOFF
      PrintSerTime(runningTimer.onTime[2], 0);
      PrintMenuNo('7');
      PrintSpacer(0);
    }
    PrintTimerUnderLine(0);
  }

  // OffTime-Line
  if (runningTimer.type.dayTimer || runningTimer.type.interval){
    // 24h & Interval
    Serial.print(F(" OffTime"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    PrintSerTime(runningTimer.offTime[0], 0);
    PrintMenuNo('2');
    PrintSpacer(0);
    if (runningTimer.type.whileON){
      // whileON
      PrintSerTime(runningTimer.offTime[1], 0);
      PrintMenuNo('5');
      PrintSpacer(0);
    }
    else if (runningTimer.type.whileOFF){
      // whileOFF exist
      Serial.print(F("   NA    ( )"));
      PrintSpacer(0);
    }  
    if (runningTimer.type.whileOFF){
      // triple
      PrintSerTime(runningTimer.offTime[2], 0);
      PrintMenuNo('8');
      PrintSpacer(0);
    }
  }

  // Offset-Line
  if (runningTimer.type.interval){
    // Interval
    PrintTimerUnderLine(0);
    Serial.print(F("  OffSet"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    PrintSerTime(runningTimer.offset[0], 0);
    PrintMenuNo('3');
    PrintSpacer(0);
    r2++; // += 2;
    if (runningTimer.type.whileON){
      // whileON
      PrintSerTime(runningTimer.offset[1], 0);
      PrintMenuNo('6');
      PrintSpacer(0);
    }
    else if (runningTimer.type.whileOFF){
      // whileOFF exist
      Serial.print(F("   NA    ( )"));
      PrintSpacer(0);
    }  
    if (runningTimer.type.whileOFF){
      // whileOFF
      PrintSerTime(runningTimer.offset[2], 0);
      PrintMenuNo('9');
      PrintSpacer(0);
    }
  }
  if (runningTimer.type.dayTimer || runningTimer.type.interval){
    // something was valid
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

  char pos = PrintMenuTop((char*)"- Edit-Timer -");
  
  PrintLine(pos++, 4, 75);
  EscLocate(4, pos);
  PrintMenuKey('0', 0, '(', ' ', 0, 0, 0);
  PrintTimerLine1(timer, 10, pos++, 1, 1);
  PrintLine(pos++, 4, 75);

  pos = pos + PrintTimerTable(timer, 11, pos);

  EscLocate(4, pos++);
  PrintMenuKey('A', 0, 0, 0, 1, (runningTimer.type.interval), 0); // && !runningTimer.type.whileON && !runningTimer.type.whileOFF), 0);
  Serial.print(F("Interval Timer     "));
  
  PrintMenuKey('B', 0, 0, 0, 1, (runningTimer.type.whileON), 0);
  Serial.print(F("whileON Timer     "));

  PrintMenuKey('C', 0, 0, 0, 1, (runningTimer.type.whileOFF), 0);
  Serial.print(F("whileOFF Timer"));
  
  EscLocate(4, pos++);
  PrintMenuKey('D', 0, 0, 0, 1, (runningTimer.type.dayTimer), 0);
  Serial.print(F("24h-Day Timer      "));

  PrintMenuKey('E', 0, 0, 0, 1, (runningTimer.type.invert), 0);
  Serial.print(F("Inverted Port     "));

  PrintMenuKey('F', 0, 0, 0, 1, (!runningTimer.type.interval && !runningTimer.type.dayTimer), 0);
  Serial.print(F("Disabled"));
  PrintShortLine(pos++, 4);
  
  EscLocate(4, pos++);
  PrintMenuKey('G', 0, 0, 0, 1, (!runningTimer.weekDays), (runningTimer.weekDays));
  Serial.print(F("ALL  "));

  PrintMenuKey('H', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 1)));
  Serial.print(F("Sun  "));

  PrintMenuKey('I', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 2)));
  Serial.print(F("Mon  "));

  PrintMenuKey('J', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 3)));
  Serial.print(F("TUE  "));

  PrintMenuKey('K', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 4)));
  Serial.print(F("Wed  "));

  PrintMenuKey('L', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 5)));
  Serial.print(F("Thu  "));

  PrintMenuKey('M', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 6)));
  Serial.print(F("Fri  "));

  PrintMenuKey('N', 0, 0, 0, 1, 0, (!bitRead(runningTimer.weekDays, 7)));
  Serial.print(F("Sat  "));  

  PrintShortLine(pos++, 4);

  EscLocate(4, pos++);
  PrintMenuKey('O', 0, 0, 0, 1, (runningTimer.state.automatic), 0);
  Serial.print(F("Automatic          "));

  PrintMenuKey('P', 0, 0, 0, 1, (runningTimer.state.permOff), 0);
  Serial.print(F("Permanent OFF      "));

  PrintMenuKey('Q', 0, 0, 0, 1, (runningTimer.state.permOn), 0);
  Serial.print(F("Permanent ON"));

  PrintShortLine(pos++, 4);

  EscLocate(4, pos++);
  PrintMenuKey('R', 0, 0, 0, 1, (runningTimer.state.tempOff), 0);
  Serial.print(F("Temporary OFF      "));

  PrintMenuKey('S', 0, 0, 0, 1, (runningTimer.state.tempOn), 0);
  Serial.print(F("Temporary ON       "));
  
  PrintMenuKeyStd('T');
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
    //runningTimer.type.whileON = 0;
    //runningTimer.type.whileOFF = 0;
    runningTimer.type.dayTimer = 0;
    break;
  case 'b':
    // Double IntervalTimer
    runningTimer.type.whileON = !runningTimer.type.whileON;
    if (runningTimer.type.whileON || runningTimer.type.whileOFF){
      runningTimer.type.interval = 1;
    }  
    // runningTimer.type.whileOFF = 0;
    runningTimer.type.dayTimer = 0;
    break;
  case 'c':
    // Triple IntervalTimer
    runningTimer.type.whileOFF = !runningTimer.type.whileOFF;
    if (runningTimer.type.whileOFF || runningTimer.type.whileON){
      runningTimer.type.interval = 1;
    }  
    // runningTimer.type.whileON = 0;
    runningTimer.type.dayTimer = 0;
    break;
  case 'd':
    // 24h Timer
    runningTimer.type.interval = 0;
    runningTimer.type.whileON = 0;
    runningTimer.type.whileOFF = 0;
    runningTimer.type.dayTimer = 1;
    break;
  case 'e':
    // Inverted
    runningTimer.type.invert = !runningTimer.type.invert;
  case 'f':
    // Disabled
    runningTimer.type.interval = 0;
    runningTimer.type.whileON = 0;
    runningTimer.type.whileOFF = 0;
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
    runningState[timer].state.tempOff = 1;
    runningState[timer].state.tempOn = 0;
    runningState[timer].state.hasChanged = 1;
    runningState[timer].tempUntil = myTime + tempTime;
    pos = -1;
    break;
  case 's':
    runningState[timer].state.tempOff = 0;
    runningState[timer].state.tempOn = 1;
    runningState[timer].state.hasChanged = 1;
    runningState[timer].tempUntil = myTime + tempTime;
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
    if (pos != 't'){
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
  byte pos = PrintMenuTop((char*)"- QuickTimer 1.02 -");
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
      PrintMenuKey((char)(65 + i), 0, 0, ' ', 1, 0, 0);
      TimerFromRomRam(i, 1);
      PrintTimerLine1(i, 9, pos++, 1, 2);
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

  char pos = PrintMenuTop((char*)"- Main Menu QuickTimer 1.02 -");
  
  uint32_t hlpDate = 0;
  uint32_t hlpTime = 0;
  
  for (int i = 0; i < RUNNING_TIMERS_CNT; i++){
    pos++;
    EscLocate(5, pos);
    PrintMenuKey((char)(65 + i), 0, 0, ' ', 1, 0, 0);
    TimerFromRomRam(i, 1);
    PrintTimerLine1(i, 0, 0, 1, 1);
  }

  pos = PrintShortLine(pos + 1, 8);

  EscLocate(5, pos);
  PrintMenuKeyStd('1');
  Serial.print(F("ReBoot"));
  EscLocate(18, pos);
  PrintMenuKeyStd('2');
  Serial.print(F("Date"));
  EscLocate(29, pos);
  PrintMenuKeyStd('3');
  Serial.print(F("Time"));
  EscLocate(40, pos);
  PrintMenuKeyStd('4');
  Serial.print(F("Address = "));
  PrintBoldValue((long)myAddress * 1000, 3, 0, '0');
  EscLocate(60, pos++);
  PrintMenuKeyStd('5');
  Serial.print(F("Speed = "));
  EscBold(1);
  Serial.print(mySpeed);
  
  EscLocate(5, pos);
  PrintMenuKeyStd('6');
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

  PrintMenuKeyStd('7');
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

  PrintMenuKey('8', 0, 0, 0, 1, (mySolarized), (!mySolarized));
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
    myAddress = GetUserVal(myAddress);
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
    mySpeed = GetUserVal(mySpeed);
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

