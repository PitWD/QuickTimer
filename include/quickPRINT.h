#ifndef QUICKPRINT_H
#define QUICKPRINT_H

#include "quicklib.h"

void PrintHlpTime(byte hourIN, byte minIN, byte secIN);
void PrintHlpDate(byte dayIN, byte monthIN, uint16_t yearIN);
void PrintSerTime(uint32_t timeIN, byte printDays, byte print);
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
    byte PrintValue(long val, byte cntLeadingZeros, byte cntDecimalPlaces, char leadingChar, byte bold);
    #define PrintBoldInt(val, cntLeadingZeros, leadingChar) PrintValue(val, cntLeadingZeros, 0, leadingChar, 1)
    #define PrintInt(val, cntLeadingZeros, leadingChar) PrintValue(val, cntLeadingZeros, 0, leadingChar, 0)
    #define PrintBoldFloat(val, cntLeadingZeros, cntDecimalPlaces, leadingChar) PrintValue(val, cntLeadingZeros, cntDecimalPlaces, leadingChar, 1)
    #define PrintFloat(val, cntLeadingZeros, cntDecimalPlaces, leadingChar) PrintValue(val, cntLeadingZeros, cntDecimalPlaces, leadingChar, 0)
#endif

void PrintErrorOK(int8_t err, byte len, char *strIN);
void PrintCentered(char *strIN, byte centerLen);
byte PrintMenuTop(char *strIN);
void PrintMenuEnd(byte pos);

void PrintMenuKey(char key, byte space, char leadChar, char trailChar, byte colon, byte bold, byte faint);
void PrintMenuKeyBoldFaint(char key, byte bold, byte faint);
void PrintMenuKeyStd(char key);
void PrintMenuNo(char number);

void PrintOFF(void);
void PrintON(void);
void PrintTrue(void);
void PrintFalse(void);
void PrintCharInSpaces(char charIN);
void PrintSpacer(byte bold);

#endif