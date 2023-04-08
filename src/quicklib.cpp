#include "quicklib.h"

// RTC-Temp
long myRtcTemp = 0;

char strHLP[STR_HLP_LEN];
char strHLP2[STR_HLP_LEN];
char strDefault[STR_HLP_LEN];
byte adrDefault;

// Check if 'speed' is a multiple of 1200
byte IsSerialSpeedValid(uint32_t speed){
    if (speed % 1200){
      return 0;
    }
    return 1;
}

// IntToStrInt / IntToStrFloat
#if SMALL_GetUserVal
  // Just Integer

  byte IntToStr_SMALL(long val, char cntLeadingChar, char leadingChar){

      // "leadingZeros MUST BE A SIGNED Variable !! "

      // int (NOT scaled by 1000 - see regular IntToStr())
      ltoa(val, strHLP, 10);
      byte len = strlen(strHLP);
 
      // Set leading zero's
      cntLeadingChar -= len;
      if (cntLeadingChar > 0){
          // space for missing zeros
          memmove(&strHLP[(int)cntLeadingChar], &strHLP[0], len);
          // set missing zeros
          memset(&strHLP[0], leadingChar, cntLeadingChar);
          // correct len
          len += cntLeadingChar;        
      }

      // set EndOfString
      strHLP[len] = 0;

      return len;
  }

#else

  byte IntToStr_BIG(long val, int8_t lz, byte dp, char lc){

      // dp = decimal places
      // lz = leading zero's !!! CAN GO NEGATIVE !!!
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

#endif

byte getBit(byte byteIN, byte bitToGet) {
    // Returns the state of the bit at position 'bitToGet' in 'byteIN'
    return (byteIN >> bitToGet) & 0x01;
}
byte setBit(byte byteIN, byte bitToSet, byte setTo) {
    // Sets the bit at position 'bitToSet' in 'byteIN' to 'setTo' 
    if (setTo) {
        byteIN |= (1 << bitToSet);
    } else {
        byteIN &= ~(1 << bitToSet);
    }
    return byteIN;
}
byte ToBCD (byte val){
  return ((val / 10) << 4) + (val % 10);
}
byte FromBCD (byte bcd){
  return (10 * ((bcd & 0xf0) >> 4)) + (bcd & 0x0f);
}
char ByteToChar(byte valIN){
    // Keeps the Bit-Pattern but NOT the VALUE
    if (valIN > 127){
        return (char)(valIN - 256);
    }
    return (char)valIN;
}

// GetUserInt, GetUserFloat
  #if SMALL_GetUserVal
    // just integer
    long GetUserInt(long valIN){
        ltoa(valIN, strHLP2, 10);
        if (GetUserString(strHLP2)){
            return atol(strHLP);
        }
        return valIN;
    }
  #else
    // floating's, too
    long GetUserVal(long defVal, int8_t type){
        // type:  0 = int as it is
        //        1 = float (*1000)
        if (type){
            // Is scaled float
            IntToFloatStr(defVal, 1, 3, ' ');
            strcpy(strHLP2, strHLP);
        }
        else{
            // Integer as it is
            ltoa(defVal, strHLP2, 10);
        }

        if (GetUserString(strHLP2)){
          // convert type into right value for StrFloatIntToInt
          if (type){
            type = -1;
          }
          defVal = StrFloatIntToInt(strHLP, type);
        }
        return defVal;  
    }
  #endif

// StrToInt (Float - GetUserVal)
#if SMALL_GetUserVal
#else
long StrFloatIntToInt(char *strIN, int8_t autoScale) {
    
    // With positive autoscale:
      // Returns 123456 for "123.45678"   (floats get scaled by 1000)
      // Returns 12345678 for "12345678"  (integers are untouched)
    // With negative autoscale:
      // All scaled by 1000
    // Without autoscale:
      // All unscaled integer

    long value = 0;

    // find the decimal point
    char *decimal = strchr(strIN, '.');
    if ((decimal == NULL) || !autoScale) {
        // no decimal point, so it's an integer value
        value = strtol(strIN, NULL, 10);
        if (autoScale < 0){
          // all scaled by 1000
          value *= 1000;
        }
    }
    else {
        // convert floating value to scaled integer
        
        // set terminator on decimal point
        *decimal = '\0';
        value = strtol(strIN, NULL, 10) * 1000;
    
        byte isNegative = 0;
        // Check if negative
        if (value < 0){
            value *= -1;
            isNegative = 1;
        }
        
        // move pointer on 1st decimal place
        decimal++;
        int factor = 100;
        while (*decimal != '\0' && factor > 0) {
            if (isdigit(*decimal)) {
                value += (*decimal - '0') * factor;
                factor /= 10;
            }
            decimal++;
        }
        if (isNegative){
            value *= -1;
        }        
    }
    return value;
}

long StrTokFloatIntToInt(char *strIN, int8_t autoScale){
    
    // extended strtok()
    // abc,123.456,xyz,789  1st call gets 123.456 and returns 123456 (floats get scaled by 1000)
    //                      next call gets 789 and returns 789 (integers are unscaled)

    static char *nextToken = NULL;  // Pointer on nextToken
    char *token;

    if (strIN != NULL) {
        // New string with value(s)
        nextToken = strIN;
    }
    byte firstVal = 1;

    while ((token = strtok(nextToken, ",")) != NULL) {
        // reset nextToken for subsequent calls to strtok()
        nextToken = NULL;  

        // check if token is a number
        if (isdigit(*token) || *token == '-') {            
            return StrFloatIntToInt(token, autoScale);
        }
    }

    // no more tokens
    return 0;
}


  /*
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
  */
#endif

byte GetUserString(char *strIN){
  
  // my tiny edlin...

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
                Print1Space();
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
          Print1Space();
          EscCursorLeft(1);
          strHLP[eos] = 0;
        }
        else if (pos && (pos < eos)){
          // Cursor is 'somewhere' - shift chars 1 to left
          pos--;
          memmove(&strHLP[pos], &strHLP[pos + 1], eos - pos + 2);
          EscCursorLeft(1);
          Serial.print(&strHLP[pos]);
          Print1Space();
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
          if (pos == STR_HLP_LEN){
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
  int8_t r = -1;         // TimeOut

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
        // Valid small Letter selected
      }
      else if (charIN > 64 && charIN < maxChar - 31){
        // Valid BIG Letter selected
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

/*
char *Fa(PGM_P strIN){
  //return strcpy_P(buf, (PGM_P)pgm_read_word(&(strIN)));
  static char buf[STR_HLP_LEN];
  return strcpy_P(buf, (PGM_P)strIN);
}
*/
