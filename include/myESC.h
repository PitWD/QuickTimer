#define fgBlack 30
#define fgRed 31
#define fgGreen 32
#define fgYellow 33
#define fgBlue 34
#define fgMagenta 35
#define fgCyan 36
#define fgWhite 37

#define bgBlack 40
#define bgRed 41
#define bgGreen 42
#define bgYellow 43
#define bgBlue 44
#define bgMagenta 45
#define bgCyan 46
#define bgWhite 47

#define fgBlackB 90
#define fgRedB 91
#define fgGreenB 92
#define fgYellowB 93
#define fgBlueB 94
#define fgMagentaB 95
#define fgCyanB 96
#define fgWhiteB 97

#define bgBlackB 100
#define bgRedB 101
#define bgGreenB 102
#define bgYellowB 103
#define bgBlueB 104
#define bgMagentaB 105
#define bgCyanB 106
#define bgWhiteB 107

#define ESC_CAN_FAINT 0
#define ESC_SOLARIZED 1
#define ESC_CAN_CURSOR 0

#if ESC_CAN_FAINT
#else
  byte escFaintDeleteColor = 39;
  char escFaintIsActive = 0;
  #if ESC_SOLARIZED
    #define fgFaint 92
  #else
    #define fgFaint 90
  #endif
#endif


void EscBold(byte set);

void EscItalic(byte set) {
	if (set) {
		// Set
		Serial.print(F("\x1B[3m"));
	}
	else {
		// Reset
		Serial.print(F("\x1B[23m"));
	}
}
void EscUnder(byte set) {
	if (set) {
		// Set
		Serial.print(F("\x1B[4m"));
	}
	else {
		// Reset
		Serial.print(F("\x1B[24m"));
	}
}

void EscLocate(byte x, byte y){
  Serial.print(F("\x1B["));
  Serial.print(y);
  Serial.print(F(";"));
  Serial.print(x);
  Serial.print(F("H"));
}
void EscCls(){
  Serial.print(F("\x1B[2J"));
}

void EscColor(byte color){
  
  if (!color){
    // Reset to Terminal Default
    color = 39;
  }

  #if ESC_CAN_FAINT
  #else  
    if (escFaintIsActive == 0){
      if(color < 40){
        // It's a ForeColor
        escFaintDeleteColor = color;
      }
    }
  #endif

  Serial.print(F("\x1B["));
  Serial.print(color);
  Serial.print(F("m"));

  if (color == 39){
    // Reset Background, too.
    EscColor(49);
  }

}

void EscFaint(byte set){

  #if ESC_CAN_FAINT
    if (set){
      Serial.print(F("\x1B[2m"));
    }
    else{
      EscBold(0);
    }
  #else
    if (set){
      escFaintIsActive = -1;
      EscColor(fgFaint);
      escFaintIsActive = 1;
    }
    else{
      escFaintIsActive = -2;
      EscColor(escFaintDeleteColor);
      escFaintIsActive = 0;
    }
  #endif

}

void EscBold(byte set){

  #if ESC_CAN_FAINT
  #else
    if (escFaintIsActive && !set){
      // We need to disable the color based faint realization
      EscFaint(0);
    }
  #endif

  if (set){
    Serial.print(F("\x1B[1m"));
  }
  else{
    Serial.print(F("\x1B[22m"));
  }   
}

void EscInverse(byte set){
 	if (set) {
		// Set
		Serial.print(F("\x1B[7m"));
	}
	else {
		// Reset
		Serial.print(F("\x1B[27m"));
	}
}
void EscCursorVisible(byte set){
	#if ESC_CAN_CURSOR
    Serial.print(F("\x1B?25"));
    if (set){
      // visible
      Serial.print(F("h"));
    }
    else{
      // invisible
      Serial.print(F("l"));
    }	
  #endif
}
void EscCursorLeft(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("D"));
}

void EscSaveCursor(){
  Serial.print(F("\0337"));
}
void EscRestoreCursor(){
  Serial.print(F("\0338"));
}

