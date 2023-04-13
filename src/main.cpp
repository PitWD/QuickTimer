#include "quicklib.h"
#include "myMenu.h"
#define WIRE Wire

//void myToRom();
//void myFromRom();

// global RunTime Timing
byte myRunSec = 0;
byte myRunMin = 0;
byte myRunHour = 0;
uint16_t myRunDay = 0;
uint32_t myRunTime = 0;

// global RealTime Timing
byte mySec = 0;
byte myMin = 0;
byte myHour = 0;
byte myDay = 1;
byte myMonth = 1;
uint16_t myYear = 2023;
uint32_t myTime = 0;


void setup() {
  // put your setup code here, to run once:

  myFromRom();

  Serial.begin(mySpeed);

  Wire.setClock(31000L);
  Wire.begin();

  RTC_GetDateTime();

// memset(runningTimers, 0, RUNNING_TIMERS_CNT * sizeof(TimerSTRUCT));
  EscCls(); EscLocate(1,1);
  for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
    
    // Clear linked temp memory
    memset(&runningState[i], 0, sizeof(TimerStateSTRUCT));
    
    // Load 'all' from eeprom
    TimerFromRomRam(i, 0);
    // Copy rom-state to ram
    TimerToRomRam(i, 0);

    // Check on validity (1st time boot...)
    if (strlen(runningTimer.name) == 0 || strlen(runningTimer.name) > 16){
      // Undefined / New
      Serial.print(F("Undefined Port/Timer : "));
      Serial.println(i + 1);
      memset(&runningTimer, 0, sizeof(TimerSTRUCT));
      strcpy(runningTimer.name, "Default-");
      runningTimer.name[8] = 'A' + i; 
      EEPROM.put(i * sizeof(TimerSTRUCT), runningTimer); 
    }
    
    // Set Output & StartValue
    pinMode(i + 2, OUTPUT);
    if (runningTimer.type.invert){
      digitalWrite(i + 2, HIGH);
      runningState[i].state.lastVal = 1;
    }
    else{
      digitalWrite(i + 2, LOW);
      runningState[i].state.lastVal = 0;
    }
  }

  PrintMainMenu();

}

void loop() {
// put your main code here, to run repeatedly:

  byte r = 1;
  byte pos = 3;
  
  // Seems, that I messed up runningState[n].state.hasChanged
  // while implementing right Back 'n' Preview times vor multi-interval...
  // firstRun is a temporary (if fix on the right place would be mor expensive)
  static byte firstRun = 1;

  if (DoTimer()){
    // A Second is over...

    if (RunTimers() || firstRun){
      // A Port has changed
      pos = 5;
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
          // Port is valid
          if (runningState[i].state.hasChanged || firstRun){
            // And has changed
            TimerFromRomRam(i, 1);
            PrintTimerLine1(i, 9, pos, 0, 2);
            digitalWrite(i + 2, runningState[i].state.lastVal);
            runningState[i].state.hasChanged = 0;
          }
          pos++; 
        }
        r = 1;
      }
    }
    
    // Print Runtime
    EscLocate(67,1);
    EscInverse(1);
    PrintRunTime();    
    // Print Realtime
    EscLocate(61,24);
    PrintDateTime();
    Serial.print(F(" "));
    //EscColor(0);
    EscInverse(0);    
    
    firstRun = 0;
  }

  if (Serial.available()){
    pos = Serial.read();
    if (pos > 96 && pos < 97 + RUNNING_TIMERS_CNT){
      PrintEditMenu(pos - 97);
      PrintLoopMenu();
    }
    else{
      PrintMainMenu();
    }
  }


}