#ifndef QUICKESC_H
#define QUICKESC_H

#include "quicklib.h"

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

// ESC / Terminal hacks
extern byte escFaintDeleteColor;
extern int8_t escFaintIsActive;
extern byte fgFaint;


void EscBold(byte set);
void EscItalic(byte set);
void EscUnder(byte set);
void EscLocate(byte x, byte y);
void EscCls();
void EscColor(byte color);
void EscFaint(byte set);
void EscInverse(byte set);
void EscCursorLeft(byte cnt);
void EscCursorRight(byte cnt);
void EscCursorUp(byte cnt);
void EscCursorDown(byte cnt);
void EscCursorUp1st(byte cnt);
void EscCursorDown1st(byte cnt);
void EscSaveCursor();
void EscRestoreCursor();

#endif

