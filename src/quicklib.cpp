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

byte GetONEchar(){

  // If SerialInput is just one char, we return this char
  // If SerialInput is more than one char we return 0
  // If there is no SerialInput we return 0

  byte r = 0;

  if (Serial.available()){
    r = Serial.read();
    delay(12);  // Wait 4 next char in a eventually trashy input
    while (Serial.available()){
      // Messy input...
      Serial.read();
      delay(12);
      r = 0;
    }    
  }

  return r;

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
    // byte firstVal = 1;

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
  //  I never underestimated a function more like this one...
  //  Getting it with selection small & stable took at least one very long day more than expected
  //  For readability it's using some redundant variables...
  //  To save flash-size I'm wasting "massive" CPU time during receiving chars.
  //  (All shared variables of all ESC-sequence-cases get new calculated - after every single char.)

  #define PRINT_ESC_DEBUG 0             // ESC-Sequences infos

  char c = 0;                           // actual received char
  byte timeOut = 60;
  byte eos = strlen(strIN);             // Position of EndOfString (termination - 00)
  byte pos = 0;                         // Position of cursor (0 = in front of 1st char)
  #define strMaxLen (STR_HLP_LEN - 1)   // max len of string without termination
  
  if (eos > strMaxLen || strIN[0] == 255){
    // Fresh flashed CPU has an messy Eprom...
    eos = 0;
    strIN[0] = 0; // Termination
  }
  
  byte sel1st = 0;                      // pos of 1st selected char
  byte selCnt = 0;                      // cnt of selected chars
  byte selPosLeft = 0;                  // 0 = cursor is right of selection / 1 = left of selection
  byte selTo1st = 0;                    // cnt of 1st selected char to pos1
  byte selToEnd = 0;                    // cnt of last selected char to end
  byte posTo1st = 0;                    // cnt of cursor to pos1
  byte posToEnd = 0;                    // cnt of cursor to end

  byte reDraw = 0;                      // redraw needed
  int8_t moveCursor = 0;                // negative to the left / positive to the right
  byte doDel = 0;                       // remove "something" from string (see mmVariables)
  char doInsert = 0;                    // insert char

  byte mmDest = 0;                      // where to memmove
  byte mmOrig = 0;                      // from where
  byte mmCnt = 0;                       // how much

  byte shift = 0;                       // Shift was pressed
  byte escCnt = 0;                      // hlp to interpret esc-sequences
  char escErr = 0;                      // hlp to interpret esc-sequences
  byte escCmd = 0;                      // hlp to interpret esc-sequences

  EscBold(1);
  Serial.print(F(">> "));
  EscBold(0);
  EscColor(fgCyan);
  EscSaveCursor();            // save start for reDraw

  if (eos){
    // Cursor at the end
    pos = eos;
    // On start the whole text is selected
    selCnt = pos;
    // Print text as selected
    EscInverse(1);
    Serial.print(strIN);
    EscInverse(0);
  }
  // We're not working with the original...
  strcpy(strHLP, strIN);
  
  // Facts are better than trust...
  strHLP[eos] = 0;

  // listen char-wise on stdIN () until ENTER
  while (c != 13){

    // Check on TimeOut
    if (DoTimer()){
      timeOut--;
      if (!timeOut){
        EscColor(0);
        return 0;
      }
    }

    if (Serial.available()){
      // New char(s) received

      // Reset TimeOut
      timeOut = 60;

      // Get the 1st char
      c = Serial.read();

      // -----------------------------------------------------------------------------------------
      // We need (parts of) this multiple times (selection cursor-move, del, back, insert)
      // Yes, it's CPU-"wasting" to do it with every incoming char, but only then it's
      // covering all kinds of ESC and non-ESC commands without wasting flash-memory
      // -----------------------------------------------------------------------------------------

        // Cursor-distances to pos1/end
        posTo1st = pos;         // Cursor results (EscCursorLeft(posTo1st)) left of 1st char 
        posToEnd = eos - pos;   // Cursor results (EscCursorRight(posToEnd)) right of last char (left of termination)
        
        // Selection-distances to pos1/end
        if (selCnt){
          // We have a selection
          
          selPosLeft = (pos == sel1st);  // If cursor is left or right of selection
          selTo1st = sel1st;
          selToEnd = eos - (sel1st + selCnt); 
          
          // when a selection needs a doDel (Del & Back & insert char)
          mmDest = sel1st;
          mmOrig = sel1st + selCnt;
          mmCnt = selToEnd + 1;
        }
        else{
          // Single Cursor without selection
          selTo1st = posTo1st;
          selToEnd = posToEnd;
        }

        // Reset all common "what happened" & "what to do" variables
        shift = 0;
        moveCursor = 0;
        reDraw = 0;
        doDel = 0;
        doInsert = 0;

      // -----------------------------------------------------------------------------------------
      // -----------------------------------------------------------------------------------------
        
      switch (c){
      case 27:
        // Take all "fast enough" following chars from a extra loop fto check on supported and unsupported ESC-Sequences
        // Single ESC is a single user-esc
  
        // Reset all ESC "what happened" & "what to do" variables
        escErr = 0;
        escCnt = 0;
        escCmd = 0;

        // Time for next char (lowest is 1200 baud - but as higher the speeds don't keep keys pressed)
        // AND TimeOut to recognize single user ESC
        delay(12);
        
        while (Serial.available()){
          // Read an ESC-Sequence

          escCnt++;
          c = Serial.read();
          delay(12);                  // max time between two chars @1200baud
                                      // no char in next round = timeout = end of unknown/broken sequence 
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'0'"));
  Print1Space();
#endif
          switch (escCnt){
          case 1:
            // check 1st (after ESC) char - check on CSI
            if (c != '['){
              // unsupported
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'1'"));
  Print1Space();
#endif
              escErr = 1;
            }
            break;
          case 2:
            // check 2nd char...
            switch (c){
            case 'A':
              // Up
            case 'B':
              // Down
            case '1':
              // pos1
              // shiftLeft / shiftRight
            case '4':
              // end
            case '3':
              // del
            case 'C':
              // right
            case 'D':
              // left
              escCmd = c;
              break;
            default:
              // unsupported
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'2'"));
  Print1Space();
#endif
              escErr = 1;
              break;
            }
            break;
          case 3:
            // check 3rd char
            switch (c){
            case '~':
              switch (escCmd){
              case '4':
                // end
              case '3':
                // del
              case '1':
                // pos1
                break;
              default:
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'3'"));
  Print1Space();
#endif
                escErr = 1;
                break;
              }
              break;
            case ';':
              // shiftLeft / shiftRight
              // shiftUp / shiftDown
              break;
            default:
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'4'"));
  Print1Space();
#endif
              escErr = 1;
              break;
            }
            break;
          case 4:
            switch (c){
            case '2':
              // shiftLeft / shiftRight
              // shiftUp / shiftDown
              break;
            default:
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'5'"));
  Print1Space();
#endif
              escErr = 1;
              break;
            }
            break;
          case 5:
            switch (c){
            case 'A':
              // Up
            case 'B':
              // Down
            case 'D':
              // shiftLeft
            case 'C':
              // shiftRight
              escCmd = c + 127;   // move 127, cause left/right without SHIFT has the same key...
              break;
            default:
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'6'"));
  Print1Space();
#endif
              escErr = 1;
              break;
            }
            break;
          default:
            // unsupported
#if PRINT_ESC_DEBUG
  Serial.print(c);
  Serial.print(F("'7'"));
  Print1Space();
#endif
            escErr = 1;
            break;
          }
        }

        if (!escCnt){
          // Single User ESC
          // Restore color and return
          EscColor(0);
          return 0;
        }
        else{
          if (!escErr){
            // Known ESC Sequence recognized
            
            switch (escCmd){
            case 'C' + 127:
              // shift right
              if (posToEnd){
                moveCursor = 1;
              }              
              shift = 1;
              break;
            case 'D' + 127:
              // shift left
              if (pos){
                moveCursor = -1;
              }              
              shift = 1;
              break;
            case 'A' + 127:
              // shift up (as substitute for shift pos-1)
              if (posTo1st){
                moveCursor = posTo1st * -1;
              }
              shift = 1;
              break;
            case 'B' + 127:
              // shift down (as substitute for shift end)
              if (posToEnd){
                moveCursor = posToEnd;
              }              
              shift = 1;
              break;
            case 'A':
              // Up (like pos1)
            case '1':
              // pos1
              moveCursor = posTo1st * -1;
              if (selCnt){
                // to start of selection
                moveCursor = (posTo1st - sel1st) * -1;
              }                          
              break;
            case 'B':
              // Down (like end)
            case '4':
              // end
              moveCursor = posToEnd;
              if (selCnt){
                // to end of selection
                moveCursor = posToEnd - selToEnd;
              }                          
              break;
            case 'C':
              // right
              if (posToEnd){
                moveCursor = 1;
              }
              if (selCnt){
                // to end of selection
                moveCursor = posToEnd - selToEnd;
              }                          
              break;
            case 'D':
              // left
              if (posTo1st){
                moveCursor = -1;
              }
              if (selCnt){
                // to start of selection
                moveCursor = (posTo1st - sel1st) * -1;
              }                          
              break;
            case '3':
              // del 
              if (!selCnt){
                // just a char (the selection case is already pre-calculated)
                if (pos < eos){
                  mmDest = pos;
                  mmOrig = pos + 1;
                  mmCnt = posToEnd;
                  selCnt = 1;
                }                
              }
              if (selCnt){
                // Delete is needed
                doDel = 1;
              }
              break;
            }

            // -----------------------------------------------------------------------------------------
            // Common variables for a valid ESC-sequence got set.
            // Look what happened & what to do related to the state before the sequence
            // -----------------------------------------------------------------------------------------

              if (!shift && selCnt && !doDel){
                // we had a selection, but we did a non-shift cursor-move => deselect & redraw..
                selCnt = 0;
                reDraw = 1;
                moveCursor += pos;
              }

              else if (shift && selCnt && moveCursor){
                // we had a selection and we are extending/reducing the selection

                // (re)use shift as absolute position of end of selection
                shift = selCnt + sel1st;
                reDraw = 1;

                if (selPosLeft ){
                  // Cursor is in front of selection
                  sel1st += moveCursor;
                  moveCursor = sel1st;  
                  if (sel1st >= shift){
                    // Flip selection from shift to... up to end
                    selCnt = sel1st - shift;
                    sel1st = shift;
                    shift = sel1st + selCnt;
                    moveCursor = shift;
                  }
                }
                else{
                  // Cursor is at the end of selection
                  shift += moveCursor;
                  moveCursor = shift;
                  if (shift <= sel1st){
                    // Flip selection from sel1st to... up to pos1
                    selCnt = sel1st - shift;
                    shift = sel1st;
                    sel1st = shift - selCnt;
                    moveCursor = sel1st;
                  }
                }
              
                // calc new size of selection
                selCnt = shift - sel1st;

              }

              else if (shift && !selCnt && moveCursor){
                // we're starting a selection
                reDraw = 1;
                if (moveCursor > 0){
                  // To the right
                  sel1st = pos;
                  selCnt = moveCursor;
                }
                else if (moveCursor < 0){
                  // To the left
                  sel1st = pos + moveCursor;
                  selCnt = moveCursor * -1;
                }
                moveCursor += pos;
              }

            // -----------------------------------------------------------------------------------------
            // -----------------------------------------------------------------------------------------
            
          }
        }
        break;
      case 8:
      case 127:
        // Back (!! we're "mis"-using/overwriting sel-variables & moveCursor !!)
        if (!selCnt){
          // just a char (the selection case is already pre-calculated)
          if (pos){
            mmDest = pos - 1;
            mmOrig = pos;
            mmCnt = posToEnd + 1;
            selCnt = 1;
          }
        }
        if (selCnt){
          // Delete is needed
          doDel = 1;
        }
        break;
      case 10:
      case 13:
        // with 13 next check in while results in return, cause "c" is still 13...
        break;
      default:
        if (c > 31 && c < 127){
          // Insert a valid char
          
          if (eos < strMaxLen || selCnt){
            // enough space in string to add the char
            doInsert = 1;
          }
          
          if (selCnt){
            // before insert we need to remove selection
            doDel = 1;
          }
          else{
            // nothing to delete - just eventually insert the char
          }
        }        
        break;
      }

      // ---------------------------------------------------------------------------
      // We're done with the char and know the eventually to-do commands...
      // reDraW & moveCursor & doDel & doInsert + linked sel-Vars & pos-Vars
      // knowing exactly what and how to do...
      // ---------------------------------------------------------------------------

        if (doDel){
          // We've something to delete from the string
          memmove(&strHLP[mmDest], &strHLP[mmOrig], mmCnt);
          reDraw = 1;             // char(s) removed - we need to redraw           
          moveCursor = mmDest;    // absolute pos of cursor after redraw
          pos = moveCursor;       // set pos here, too! to do following "doInsert" right
          eos -= selCnt;
          selCnt = 0;
        }

        if (doInsert){
          // char to insert
          if (pos < eos){
            // Cursor is 'somewhere' - shift chars 1 to the right
            memmove(&strHLP[pos + 1], &strHLP[pos], posToEnd + 1);
          }
          else{
            // Cursor (pos) & eos at the 1st or last position of string
            // termination needed
            strHLP[pos + 1] = 0;
          }
          strHLP[pos] = c;
          moveCursor = pos + 1;
          eos++;
          reDraw = 1;
        }

        if (reDraw){
          // we have to redraw the line

          EscRestoreCursor();
          
          if (!selCnt){
            // without selection - adjust sel1st (kind of a mis-use of sel1st)
            sel1st = eos;
          }

          if (sel1st){
            // unselected text in front of selection (or full unselected text)
            for (byte i = 0; i < sel1st; i++){
              Serial.print(strHLP[i]);
            }
          }              
    
          if (selCnt){
            // we have selected text
            EscInverse(1);
            for (byte i = sel1st; i < sel1st + selCnt; i++){
              Serial.print(strHLP[i]);
            }
            EscInverse(0);
            // unselected text after selection
            for (byte i = sel1st + selCnt; i < eos; i++){
              Serial.print(strHLP[i]);
            }
          }

          // Delete possibly too much chars from previous print
          PrintSpaces(strMaxLen - eos);
          
          // Place Cursor on front again... to be right for an eventually moveCurser
          EscRestoreCursor();
          pos = 0;
        }

        if (moveCursor){
          // we need to move the cursor

          pos += moveCursor;

          if (moveCursor > 0){
            // Right
            EscCursorRight(moveCursor);
          }
          else{
            // Left
            EscCursorLeft(moveCursor * -1);
          }
        }
      // ---------------------------------------------------------------------------
      // ---------------------------------------------------------------------------

    }
    // Nothing in buffer (wait for char / timeout)
  }

  // Enter got pressed - RETURN

  // Facts are better than trust...
  strHLP[eos] = 0;
  // Reset color
  EscColor(0);
  // copy result into original
  strcpy(strIN, strHLP);
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
    charIN = GetONEchar();
    if (charIN){
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
      else if (charIN == 13 || charIN == 27){
        // Enter - Exit - Back
        r = 0;
      }
      else if (charIN == '+' || charIN == '-' || charIN == '<' || charIN == '>'){
        // Up/Down
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

