#ifndef QUICKLIB_H
#define QUICKLIB_H

// True = No floating-point support...
#define SMALL_GetUserVal 1  

// IIC IO/Buffer & Global HLP Strings
#define IIC_STR_LEN 34
#define STR_HLP_LEN 17
#define USE_ATLAS_PRTCL 0


#include <Arduino.h>
#include <Wire.h>
#include "quickESC.h"
#include "quickTIME.h"
#include "quickIIC.h"
#include "quickPRINT.h"

// Seconds between RTC sync's (0 disables sync
#define syncRTCinterval 86400L

// ESC / Terminal hacks
#define ESC_CAN_FAINT 0
#define ESC_SOLARIZED 1

// Declarations Declarations Declarations Declarations Declarations Declarations Declarations Declarations

byte IsSerialSpeedValid(uint32_t speed);

#if SMALL_GetUserVal
  byte IntToStr_SMALL(long val, char cntLeadingChar, char leadingChar);
  #define IntToIntStr(val, cntLeadingChar, leadingChar) IntToStr_SMALL(val, cntLeadingChar, leadingChar)
#else
  byte IntToStr_BIG(long val, char lz, byte dp, char lc);
  #define IntToIntStr(val, cntLeadingChar, leadingChar) IntToStr_BIG(val * 1000, cntLeadingChar, 0, leadingChar)
  #define IntToFloatStr(val, cntLeadingChar, cntDecimalPlaces, leadingChar) IntToStr_BIG(val, cntLeadingChar, cntDecimalPlaces, leadingChar)
#endif

byte getBit(byte byteIN, byte bitToGet);
byte setBit(byte byteIN, byte bitToSet, byte setTo);

#if SMALL_GetUserVal
    // just integer
    long GetUserInt(long valIN);
#else
    long GetUserVal(long defVal, byte type);
    #define GetUserInt(valIN) GetUserVal(valIN, 0)
    #define GetUserFloat(valIN) GetUserVal(valIN, 1)
#endif      
#if SMALL_GetUserVal
#else
  //long StrToInt(char *strIN, byte next);
  long StrTokFloatIntToInt(char *strIN);
  long StrFloatIntToInt(char *strIN);
#endif

byte GetUserString(char *strIN);

byte ToBCD (byte val);
byte FromBCD (byte bcd);
char ByteToChar(byte valIN);

uint32_t GetUserTime(uint32_t timeIN);
uint32_t GetUserDate(uint32_t timeIN);

char GetUserKey(byte maxChar, byte noCnt);


// Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals Globals

// RTC-Temp
extern long myRtcTemp;

// global IIC I/O buffer and HLP-Strings
extern char iicStr[IIC_STR_LEN];
extern char strHLP[STR_HLP_LEN];
extern char strHLP2[STR_HLP_LEN];
extern char strDefault[STR_HLP_LEN];

// ModBusAddress
extern byte myAddress;


#endif /* QUICKLIB_H */