#ifndef QUICKPRINT_H
#define QUICKPRINT_H

#include "quicklib.h"

void PrintHlpTime(byte hourIN, byte minIN, byte secIN);
void PrintHlpDate(byte dayIN, byte monthIN, uint16_t yearIN);
void PrintSerTime(uint32_t timeIN, byte printDays);
void PrintTime();
void PrintRunTime();
void PrintDateTime();

void PrintCharsCnt(char charToPrint, byte cnt);
#define PrintSpaces(cnt) PrintCharsCnt(' ', cnt)
void Print1Space(void);

byte PrintLine(byte pos, byte start, byte len);
#define PrintShortLine(posY, posX) PrintLine(posY, posX, 3)

#if SMALL_GetUserVal
    byte PrintBoldValue(long val, byte cntLeadingZeros, char leadingChar);
    #define PrintBoldInt(val, cntLeadingZeros, leadingChar) PrintBoldValue(val, cntLeadingZeros, leadingChar)
#else
    byte PrintBoldValue(long val, byte cntLeadingZeros, byte cntDecimalPlaces, char leadingChar);
    #define PrintBoldInt(val, cntLeadingZeros, leadingChar) PrintBoldValue(val, cntLeadingZeros, 0, leadingChar)
    #define PrintBoldFloat(val, cntLeadingZeros, cntDecimalPlaces, leadingChar) PrintBoldValue(val, cntLeadingZeros, cntDecimalPlaces, leadingChar)
#endif

void PrintErrorOK(char err, char ezo, char *strIN);
byte PrintMenuTop(char *strIN);
void PrintMenuEnd(byte pos);

void PrintMenuKey(char key, byte space, char leadChar, char trailChar, byte colon, byte bold, byte faint);
void PrintMenuKeyStd(char key);
void PrintMenuNo(char number);

void PrintOFF(void);
void PrintON(void);

void PrintSpacer(byte bold);

#endif