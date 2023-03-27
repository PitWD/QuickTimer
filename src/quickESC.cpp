#include "quickESC.h"

// ESC / Terminal hacks
byte escFaintDeleteColor = 39;
char escFaintIsActive = 0;
#if ESC_SOLARIZED
  byte fgFaint = 92;
#else
  byte fgFaint = 90;
#endif

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
  EscColor(0);
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
    //if (escFaintIsActive && !set){
    if (escFaintIsActive){
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
void EscCursorLeft(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("D"));
}
void EscCursorRight(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("C"));
}
void EscCursorUp(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("A"));
}
void EscCursorDown(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("B"));
}
void EscCursorUp1st(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("F"));
}
void EscCursorDown1st(byte cnt){
    Serial.print(F("\x1B["));
    Serial.print(cnt);
    Serial.print(F("E"));
}

void EscSaveCursor(){
  Serial.print(F("\0337"));
}
void EscRestoreCursor(){
  Serial.print(F("\0338"));
}

