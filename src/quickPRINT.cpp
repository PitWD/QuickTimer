
#include "quickPRINT.h"

void PrintHlpTime(byte hourIN, byte minIN, byte secIN){
    DayTimeToStr(hourIN, minIN, secIN);
    Serial.print(strHLP2);
}

void PrintHlpDate(byte dayIN, byte monthIN, uint16_t yearIN){
    DateToStr(dayIN, monthIN, yearIN);
    Serial.print(strHLP2);
}

void PrintSerTime(uint32_t timeIN, byte printDays, byte print){
  // if "printDays" is set, '000d 00:00:00'
  // else         '00:00:00'

  SerialDayTimeToStr(timeIN);           // strHLP2 contains 00:00:00 - strHLP was used....

  if (printDays){
    uint32_t days = timeIN / 86400UL;
    IntToIntStr(days, 3, ' ');          // strHLP contains 000
    memmove(&strHLP2[5], strHLP2, 9);   // make space for "000d "
    memcpy(strHLP2, strHLP, 3);
    strHLP2[3] = 'd';
    strHLP2[4] = ' ';
    // Serial.print(strHLP);
    // Serial.print(F("d "));
  }
  
  if (print){
    Serial.print(strHLP2);
  }
  
}

void PrintTime(){
    PrintHlpTime(myHour, myMin, mySec);
}

void PrintRunTime(){
    PrintSerTime(myRunTime, 1, 1);
}

void PrintDateTime(){
    PrintHlpDate(myDay, myMonth, myYear);
    Print1Space();
    PrintHlpTime(myHour, myMin, mySec);
}

// Print char 'charToPrint' 'cnt' times
void PrintCharsCnt(char charToPrint, byte cnt){
  while (cnt--){
    Serial.print(charToPrint);
  }
}
void PrintSpaces(byte cnt){
  PrintCharsCnt(' ', cnt);
}
// Print One Space
void Print1Space(void){
    PrintSpaces(1);
}

byte PrintLine(byte posY, byte posX, byte len){
  if (posY && posX){
    EscLocate(posX, posY++);
  }
  PrintCharsCnt('-', len);
  
  return posY;
}

byte PrintDashLine(byte posY, byte posX, byte len){
  if (posY && posX){
    EscLocate(posX, posY++);
  }
  len /= 2;
  while (len--){
    Serial.print(F("- "));
  }
  return posY;
}


#if SMALL_GetUserVal
  byte PrintBoldValue(long val, byte cntLeadingZeros, char leadingChar){
    
    EscBold(1);
    
    byte r = IntToIntStr(val, cntLeadingZeros, leadingChar);

    Serial.print(strHLP);
    EscBold(0);

    return r;
  }
#else

  byte PrintValue(long val, byte cntLeadingZeros, byte cntDecimalPlaces, char leadingChar, byte bold){
    
    byte r = 0;
    
    EscBold(bold);
      
    if (cntDecimalPlaces){
      r = IntToFloatStr(val, cntLeadingZeros, cntDecimalPlaces, leadingChar);
    }
    else{
      r = IntToIntStr(val, cntLeadingZeros, leadingChar);
    }

    Serial.print(strHLP);
    EscBold(0);

    return r;
  }

#endif


void PrintErrorOK(int8_t err, byte len, char *strIN){

   // !! Bottom-Line of TUI !!

  // Err: 0 = info, -1 = err, 1 = OK
  // Err: 0 = black, -1 = red, 1 = green

  //byte len = strlen(strIN) + 40;

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
  
  PrintSpaces(4);
  //Serial.print(F("    "));
  Serial.print(strIN);
  EscBold(0);
  EscColor(49);

  Serial.print(F(" @ "));
  PrintRunTime();

  // c++ pointer shit...
  if (!len){
    // TAKE CARE
    // if strIN is a char-array a la strHLP - we already have the len of strIN
    // if not, function got called with a (char*)"BlaBlaBla"
    len = strlen(strIN);
  }
  
  PrintSpaces(40 - len);
  //for (int i = 0; i < len; i++){
    //Print1Space();
  //}
  
  //PrintDateTime();
  //Print1Space();
  
  EscInverse(0);
//Serial.println("");
//Serial.println(strlen(strIN));

}

void PrintCentered(char *strIN, byte centerLen){
  byte spaces = centerLen - strlen(strIN);
  byte frontSpaces = spaces / 2;
  spaces -= frontSpaces;
  PrintSpaces(frontSpaces);
  Serial.print(strIN);
  PrintSpaces(spaces);
}

byte PrintMenuTop(char *strIN){

  EscCls();
  EscColor(0);
  EscInverse(1);
  EscLocate(1, 1);
  EscBold(1);
  PrintCentered(strIN, 80);
  EscBold(0);
  EscInverse(0);
  return 2;

}
void PrintMenuEnd(byte pos){
  Serial.println(F("\n"));
  Serial.print(F("    Select key, or Enter(for return)..."));
}
void PrintLoopTimes(){
    // Print Runtime
    EscLocate(67,1);
    EscInverse(1);
    PrintRunTime();    
    // Print Realtime
    EscLocate(61,24);
    PrintDateTime();
    Serial.print(F(" "));
    EscInverse(0);    
}

void PrintMenuKey(char key, byte space, char leadChar, char trailChar, byte colon, byte bold, byte faint){
  // "space" is a leading space - (as very 1st print)
  // "key" is all time bold and underlined followed by a ')'
  // "colon" ': ' (bold, underlined) is an option
  // "leadChar" (bold, underlined) is an option
  // "trailChar" (not bold, nor underlined) is an option
  // "bold" sets bold on exit
  // "faint" sets faint on exit
  // if bold and faint is true... don't do ESCKeyStyle (Hack to get faint (or whatever) - see i.e. manual-menu QuickWater)

  if (space){
    Print1Space();
  }
  if (!(bold && faint)){
    EscKeyStyle(1);
  }
  
  if (leadChar){
    Serial.print(leadChar);
  }
  Serial.print(key);
  Serial.print(F(")"));
  if (colon){
    Serial.print(F(":"));
  }
  EscBold(0);
  EscUnder(0);
  if (colon){
    Print1Space();
  }
  if (trailChar){
    Serial.print(trailChar);
  }
  EscColor(0);
  if (bold && !faint){
    EscBold(1);
  }
  else if (faint && !bold){
    EscFaint(1);
  }
  
}
void PrintMenuKeyStdBoldFaint(char key, byte bold, byte faint){
  // "A): "
  PrintMenuKey(key, 0, 0, 0, 1, bold, faint);
/*
  PrintMenuKey((char)(ezo + 49), 0, '(', 0, 0, bold, !bold);
  PrintMenuKey(i + 'o', 0, '(', ' ', 0, 1, 0);
  PrintMenuKey(i + 'A', 1, '(', 0, 0, 0, 0);
  PrintMenuKey(i + 'G', 1, '(', 0, 0, 0, 0);

*/
}
void PrintMenuKeyStd(char key){
   // "A): "
  PrintMenuKey(key, 0, 0, 0, 1, 0, 0);
}
void PrintMenuKeySmall(char key){
  // "A) "
  PrintMenuKey(key, 0, 0, ' ', 0, 0, 0);
}
void PrintMenuKeySmallBoldFaint(char key, byte bold, byte faint){
  // "A) "
  PrintMenuKey(key, 0, 0, ' ', 0, bold, faint);
}
void PrintMenuKeyBracketed(char key){
  // " (A)"
  PrintMenuKey(key, 1, '(', 0, 0, 0, 0);
}

void PrintOFF(void){
  Serial.print(F("OFF"));
}
void PrintON(void){
  Serial.print(F(" ON"));
}
void PrintTrue(void){
  Serial.print(F(" True"));
}
void PrintFalse(void){
  Serial.print(F("False"));
}
void PrintCharInSpaces(char charIN){
  Print1Space();
  Serial.print(charIN);
  Print1Space();
}

void PrintSpacer(byte bold){
  EscBold(0);
  Serial.print(F(" | "));
  if (bold){
    EscBold(1);
  }
}

