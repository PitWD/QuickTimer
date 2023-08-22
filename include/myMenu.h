
#ifndef MYMENU_H
#define MYMENU_H

#include "quicklib.h"
#include <EEPROM.h>
#include <myTime.h>

void (*softReset)(void) = 0;

// my Eeprom - Variables

/*
byte myBoot = 0;    // 0 = Terminal  /  1 = Slave
uint32_t mySpeed = 9600;
byte mySolarized = 0;
byte myAddress = 123;
*/

byte firstRun = 1;

mySTRUCT my;

void myToRom(){
  EEPROM.put(997, my);
  // 1011 is next...
}
void myFromRom(){
  // 1011 is next...
  EEPROM.get(997, my);
  if (!IsSerialSpeedValid(my.Speed)){
    my.Speed = 9600;
  }
  /*
  if (!myAddress || myAddress > 254){
    myAddress = 123;
  }
  */
  
  fgFaint = my.Solarized;

}

void PrintTimerLine1(byte timerID, byte posX, byte posY, byte printName, byte printType){

  // "timerID"      - the timer-id
  // "posX & posY"  - if set, set cursor position
  // "printName"    - if the 'static' name is printed
  // "printType"    - 0 = type-area untouched
  //                - 1 = timer-type
  //                - 2 = last set/change/temp-start/end time

  if (posX && posY){
    EscLocate(posX, posY);
  }
    
  // Name 16 chars
  if (printName){
    EscBold(1);
    Serial.print(runningTimer.name);
    EscBold(0);
    PrintSpaces(16 - strlen(runningTimer.name));
    Serial.print(F("| "));
  }
  else{
    EscCursorRight(18);
  }  

  // Type 11 chars
  if (printType == 1){
    // Print Timer-Type
    if (runningTimer.type.dayTimer == 1){
      //PrintSpaces(2);
      Serial.print(F("  24h   "));
    }
    else if (runningTimer.type.interval){
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
      else{
        // just Interval
        Serial.print(F("Interval"));
      } 
    }    
    else{
      EscFaint(1);
      Serial.print(F("Disabled"));
    }
  }
  else if (printType == 2){
    // Print Time
    PrintSerTime(myTime, 0, 1);
  }
  else{
    // Print Nothing
    EscCursorRight(8);
  }
  EscFaint(0);
  PrintSpacer(0);
  
  // Check on automatic permanent & temporary state 9 chars
  if (runningTimer.state.permOff){
    EscBoldColor(fgRed);
      // EscBold(1);
      // EscColor(fgRed);
    Serial.print(F("perm.OFF"));
  }
  else if (runningTimer.state.permOn){
    EscBoldColor(fgGreen);
      // EscBold(1);
      // EscColor(fgGreen);
    Serial.print(F("perm. ON"));
  }
  else if (runningTimer.state.tempOff && myTime < runningState[timerID].tempUntil){
    EscFaint(1);
    EscColor(fgRed);
    Serial.print(F("temp.OFF"));
  }
  else if (runningTimer.state.tempOn && myTime < runningState[timerID].tempUntil){
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
  EscBoldColor(0);
    // EscBold(0);
    // EscColor(0);
  PrintSpacer(0);


  // On / Off / N/A 3 chars
  if (runningTimer.state.permOff){
    EscBoldColor(fgRed);
      // EscBold(1);
      // EscColor(fgRed);
    PrintOFF();
  }
  else if (runningTimer.state.permOn){
    EscBoldColor(fgGreen);
      // EscBold(1);
      // EscColor(fgGreen);
    PrintON();
  }
  else if (runningTimer.state.tempOff && myTime < runningState[timerID].tempUntil){
    EscFaint(1);
    EscColor(fgRed);
    PrintOFF();
  }
  else if (runningTimer.state.tempOn && myTime < runningState[timerID].tempUntil){
    EscFaint(1);
    EscColor(fgGreen);
    PrintON();
  }
  else if (runningTimer.state.automatic){
    if (runningTimer.state.lastVal){
      EscColor(fgGreen);
      PrintON();
    }
    else{
      EscColor(fgRed);
      PrintOFF();
    }
  }
  else{
    // Fully disabled
    EscFaint(1);
    Serial.print(F("N/A"));
  }
  EscBoldColor(0);
    // EscBold(0);
    // EscColor(0);
  PrintSpacer(0);

  // Last & Next Action...
  if (runningTimer.state.permOn || runningTimer.state.permOff){
    // Permanent On/Off
    EscFaint(1);
    PrintSpaces(8);
    Serial.print(F("N/A"));
    PrintSpaces(8);
    PrintSpacer(0);
  }
  else if (runningTimer.state.tempOn || runningTimer.state.tempOff){
    // Temporary On/Off
    PrintSpaces(5);
    //Serial.print(F("     "));
    PrintSerTime(runningState[timerID].tempUntil, 0, 1);
    PrintSpaces(6);
    //Serial.print(F("      "));
    PrintSpacer(0);
  }
  else if (runningTimer.state.automatic){
    // Automatic
    if (runningTimer.state.lastVal){
      // On since -> until
      EscColor(fgGreen);
      PrintSerTime(runningState[timerID].lastAction, 0, 1);
      EscColor(39);
      Serial.print(F(" - "));
      EscColor(fgRed);
    }
    else{
      // Off since -> until
      EscColor(fgRed);
      PrintSerTime(runningState[timerID].lastAction, 0, 1);
      EscColor(39);
      Serial.print(F(" - "));
      EscColor(fgGreen);
    }
    PrintSerTime(runningState[timerID].nextAction, 0, 1);
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

void PrintTimerTableNA(void){
  Serial.print(F("   NA    ( )"));
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
    if (runningTimer.type.dayTimer){
      // !! 24h Day Timer !!
      PrintSerTime(runningTimer.offset[0], 0, 1);
    }
    else{
      PrintSerTime(runningTimer.onTime[0], 0, 1);
    }
    
    PrintMenuKeyBracketed('1');
    PrintSpacer(0);
    if (runningTimer.type.whileON){
      // whileON
      PrintSerTime(runningTimer.onTime[1], 0, 1);
      PrintMenuKeyBracketed('4');
      PrintSpacer(0);
    }
    else if (runningTimer.type.whileOFF){
      // whileOFF exist
      PrintTimerTableNA();
      PrintSpacer(0);
    }  
    if (runningTimer.type.whileOFF){
      // whileOFF
      PrintSerTime(runningTimer.onTime[2], 0, 1);
      PrintMenuKeyBracketed('7');
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
    if (runningTimer.type.dayTimer){
      PrintSerTime(runningTimer.realOffTime, 0, 1);
    }
    else{
      PrintSerTime(runningTimer.offTime[0], 0, 1);
    }
        
    PrintMenuKeyBracketed('2');
    PrintSpacer(0);
    if (runningTimer.type.whileON){
      // whileON
      PrintSerTime(runningTimer.offTime[1], 0, 1);
      PrintMenuKeyBracketed('5');
      PrintSpacer(0);
    }
    else if (runningTimer.type.whileOFF){
      // whileOFF exist
      PrintTimerTableNA();
      PrintSpacer(0);
    }  
    if (runningTimer.type.whileOFF){
      // triple
      PrintSerTime(runningTimer.offTime[2], 0, 1);
      PrintMenuKeyBracketed('8');
      PrintSpacer(0);
    }
  }

  // Offset-Line
  if (runningTimer.type.interval || runningTimer.type.dayTimer){
    // Interval
    PrintTimerUnderLine(0);
    Serial.print(F("  OffSet"));
    EscBold(0);
    Serial.print(F(":"));
    PrintSpacer(0);
    if (runningTimer.type.dayTimer){
      PrintTimerTableNA();
    }
    else{
      PrintSerTime(runningTimer.offset[0], 0, 1);
      PrintMenuKeyBracketed('3');
    }
    PrintSpacer(0);
    r2++; // += 2;
    if (runningTimer.type.whileON){
      // whileON
      PrintSerTime(runningTimer.offset[1], 0, 1);
      PrintMenuKeyBracketed('6');
      PrintSpacer(0);  
  }
    else if (runningTimer.type.whileOFF){
      // whileOFF exist
      PrintTimerTableNA();
      PrintSpacer(0);  
    }
    if (runningTimer.type.whileOFF){
      // whileOFF
      PrintSerTime(runningTimer.offset[2], 0, 1);
      PrintMenuKeyBracketed('9');
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
  PrintMenuKeyStdBoldFaint('a', (runningTimer.type.interval), 0); // && !runningTimer.type.whileON && !runningTimer.type.whileOFF), 0);
  Serial.print(F("Interval Timer     "));

  PrintMenuKeyStdBoldFaint('b', (runningTimer.type.whileON), 0);
  Serial.print(F("whileON Timer     "));

  PrintMenuKeyStdBoldFaint('c', (runningTimer.type.whileOFF), 0);
  Serial.print(F("whileOFF Timer"));
  
  EscLocate(4, pos++);
  PrintMenuKeyStdBoldFaint('d', (runningTimer.type.dayTimer), 0);
  Serial.print(F("24h-Day Timer      "));

  PrintMenuKeyStdBoldFaint('e', (runningTimer.type.invert), 0);
  Serial.print(F("Inverted Port     "));


  PrintMenuKeyStdBoldFaint('f', (!runningTimer.type.interval && !runningTimer.type.dayTimer), 0);
  Serial.print(F("Disabled"));
  PrintShortLine(pos++, 4);
  
  EscLocate(4, pos++);
  PrintMenuKeyStdBoldFaint('g', !runningTimer.weekDays, runningTimer.weekDays);
  Serial.print(F("ALL  "));

  PrintMenuKeyStdBoldFaint('h', bitRead(runningTimer.weekDays, 1), !bitRead(runningTimer.weekDays, 1));
  Serial.print(F("Sun  "));

  PrintMenuKeyStdBoldFaint('i', bitRead(runningTimer.weekDays, 2), !bitRead(runningTimer.weekDays, 2));
  Serial.print(F("Mon  "));

  PrintMenuKeyStdBoldFaint('j', bitRead(runningTimer.weekDays, 3), !bitRead(runningTimer.weekDays, 3));
  Serial.print(F("Tue  "));

  PrintMenuKeyStdBoldFaint('k', bitRead(runningTimer.weekDays, 4), !bitRead(runningTimer.weekDays, 4));
  Serial.print(F("Wed  "));

  PrintMenuKeyStdBoldFaint('l', bitRead(runningTimer.weekDays, 5), !bitRead(runningTimer.weekDays, 5));
  Serial.print(F("Thu  "));

  PrintMenuKeyStdBoldFaint('m', bitRead(runningTimer.weekDays, 6), !bitRead(runningTimer.weekDays, 6));
  Serial.print(F("Fri  "));

  PrintMenuKeyStdBoldFaint('n', bitRead(runningTimer.weekDays, 7), !bitRead(runningTimer.weekDays, 7));
  Serial.print(F("Sat  "));  

  PrintShortLine(pos++, 4);

  EscLocate(4, pos++);
  PrintMenuKeyStdBoldFaint('o', (runningTimer.state.automatic), 0);
  Serial.print(F("Automatic"));
  PrintSpaces(10);

  PrintMenuKeyStdBoldFaint('p', (runningTimer.state.permOff), 0);
  Serial.print(F("Permanent OFF"));
  PrintSpaces(6);

  PrintMenuKeyStdBoldFaint('q', (runningTimer.state.permOn), 0);
  Serial.print(F("Permanent ON"));

  PrintShortLine(pos++, 4);

  EscLocate(4, pos++);
  PrintMenuKeyStdBoldFaint('r', (runningTimer.state.tempOff), 0);
  Serial.print(F("Temporary OFF"));
  PrintSpaces(6);

  PrintMenuKeyStdBoldFaint('s', (runningTimer.state.tempOn), 0);
  Serial.print(F("Temporary ON"));
  PrintSpaces(7);

  PrintMenuKeyStd('t');
  Serial.print(F("SetTempTime: "));
  EscBold(1);
  PrintSerTime(tempTime, 0, 1);
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
    if (runningTimer.type.dayTimer){
      runningTimer.offset[0] = GetUserTime(runningTimer.offset[0]);
    }
    else{
      runningTimer.onTime[0] = GetUserTime(runningTimer.onTime[0]);
    }    
    break;
  case '2':
    // OffTime
    if (runningTimer.type.dayTimer){
      runningTimer.realOffTime = GetUserTime(runningTimer.realOffTime);
    }
    else{
      runningTimer.offTime[0] = GetUserTime(runningTimer.offTime[0]);
    }
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
    if ((runningTimer.type.whileON || runningTimer.type.whileOFF) && !runningTimer.type.dayTimer){
      runningTimer.type.interval = 1;
    }  
    // runningTimer.type.whileOFF = 0;
    //day//runningTimer.type.dayTimer = 0;
    break;
  case 'c':
    // Triple IntervalTimer
    runningTimer.type.whileOFF = !runningTimer.type.whileOFF;
    if ((runningTimer.type.whileOFF || runningTimer.type.whileON) && !runningTimer.type.dayTimer){
      runningTimer.type.interval = 1;
    }  
    // runningTimer.type.whileON = 0;
    //day//runningTimer.type.dayTimer = 0;
    break;
  case 'd':
    // 24h Timer
    runningTimer.type.interval = 0;
    //day//runningTimer.type.whileON = 0;
    //day//runningTimer.type.whileOFF = 0;
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
  case 'h' ... 'n':
    // Sunday to Saturday
    runningTimer.weekDays = setBit(runningTimer.weekDays, pos - 'g', !getBit(runningTimer.weekDays, pos - 'g'));
    break;
  /*
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
  */
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
  if (runningTimer.type.dayTimer && (pos == '1' || pos == '2' || pos == 'd')){
    // Calc 24Day Timer Values into Interval + offset
    if (runningTimer.realOffTime < runningTimer.offset[0]){
      // Jump over Midnight
      runningTimer.onTime[0] = (86400L - runningTimer.offset[0]) + runningTimer.realOffTime;
    }
    else{
      runningTimer.onTime[0] = runningTimer.realOffTime - runningTimer.offset[0];
    }
    
    runningTimer.offTime[0] = 86400L - runningTimer.onTime[0];

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

byte PrintQuickTimer(){
  return PrintMenuTop((char*)"- QuickTimer 1.07 -");
}

void PrintLoopMenu(){

  if (!my.Boot){
    byte r = 1;
    byte pos = PrintQuickTimer();

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
        PrintMenuKey(i + 'a', 0, 0, ' ', 1, 0, 0);
        TimerFromRomRam(i, 1);
        PrintTimerLine1(i, 9, pos++, 1, 2);
      }
      r = 1;
    }
    
    PrintErrorOK(0, 0 ,(char*)"Loop started...", 0);

    EscBold(1);
    pos = PrintLine(pos, 5, 71);
    EscBold(0);
  }
  else if (my.Boot < 3){
    // ModBus RTU & AscII
  }
  else{
    // just values
    firstRun = 1;
  }

}

void PrintMainMenu(){

Start:

  // To see right Last/Next after StartUp und changes...
  RunTimers();

  char pos = PrintQuickTimer();
  
  uint32_t hlpDate = 0;
  uint32_t hlpTime = 0;
  
  for (int i = 0; i < RUNNING_TIMERS_CNT; i++){
    pos++;
    EscLocate(5, pos);
    PrintMenuKey(i + 'a', 0, 0, ' ', 1, 0, 0);
    TimerFromRomRam(i, 1);
    PrintTimerLine1(i, 0, 0, 1, 1);
  }

  pos = PrintShortLine(pos + 1, 8);

  EscLocate(5, pos);
  PrintMenuKeyStd('1'); Serial.print(F("ReBoot   "));
  PrintMenuKeyStd('2'); Serial.print(F("Date   "));
  PrintMenuKeyStd('3'); Serial.print(F("Time   "));
  PrintMenuKeyStd('4'); Serial.print(F("Address = "));
  PrintBoldValue(my.Address, 3, '0');
  PrintSpaces(3);
  PrintMenuKeyStd('5'); Serial.print(F("Speed = "));
  EscBold(1);
  Serial.print(my.Speed);
  
  EscLocate(5, pos + 1);
  PrintMenuKeyStd('6'); Serial.print(F("BootMode = "));
  EscBold(1);
  Serial.print((char*)Fa(bootMode[my.Boot]));
  PrintSpaces(3);
  
  
  //PrintMenuKeyStdBoldFaint('6', !my.Boot, my.Boot); Serial.print(F("Boot4Terminal"));
  //PrintSpaces(3);
  //PrintMenuKeyStdBoldFaint('7', my.Boot, !my.Boot); Serial.print(F("Boot4ModBUS"));

  PrintMenuKeyStd('7'); Serial.print(F("Dim"));
  EscFaint(1);
  Serial.print(F("Color   "));
  EscFaint(0);
  PrintMenuKeyStd('8'); Serial.print(F("KeyColor"));

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
    softReset();
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
    my.Address = GetUserInt(my.Address);
    if (!my.Address || my.Address > 254){
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
    my.Speed = GetUserInt(my.Speed);
    if (IsSerialSpeedValid(my.Speed)){ 
      // valid - save to eeprom
      myToRom();
    }
    else{
      // illegal - reload from eeprom
      myFromRom();
    }
    break;    
  case '6':
    // Boot Terminal / Boot ModBus
    my.Boot++;
    if (my.Boot > 3){
      my.Boot = 0;
    }
    myToRom();
    break;
  //case '7':
    // Boot Slave
    //my.Boot = 1;
    //myToRom();
    //break;
  case '7':
    // Solarized
    my.Solarized = EscGetNextColor(my.Solarized);
    fgFaint = my.Solarized;
    myToRom();
    break;
  case '8':
    // KeyColor
    my.KeyColor = EscGetNextColor(my.KeyColor);
    myToRom();
    break;    
  case '0':
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


#endif