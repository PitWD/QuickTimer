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

  Serial.begin(my.Speed);

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

  if (DoTimer()){
    // A Second is over...

    if (RunTimers() || firstRun){
      // A Port has changed
      pos = 5;
      byte state = 0;
      for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
        // Check state  
        if (runningState[i].state.permOff){   
        }
        else if (runningState[i].state.permOn){  
          state = 1;
        }
        else if (runningState[i].state.tempOff && myTime < runningState[i].tempUntil){   
          state = 2;
        }
        else if (runningState[i].state.tempOn && myTime < runningState[i].tempUntil){ 
          state = 3;
        }
        else if (runningState[i].state.automatic){
          state = 4;
        }
        else{
          // Fully disabled
          state = 5;
          r = 0;
        }
        if (r){
          // Port is valid
          if (runningState[i].state.hasChanged || firstRun){
            // And has changed
            TimerFromRomRam(i, 1);
            if (!my.Boot){
              // Terminal
              PrintTimerLine1(i, 9, pos, 0, 2);
            }
            else if (my.Boot < 3){
              // ModBus RTU & AscII
            }
            else{
              // just values
              MBaction(my.Address, i, state, runningState[i].state.lastVal);
              
              // Type of timer
              byte type = 0;
              if (runningTimer.type.dayTimer == 1){
                //PrintSpaces(2);
                type = 1;
              }
              else if (runningTimer.type.interval){
                if (runningTimer.type.whileON && runningTimer.type.whileOFF){
                  // whileOn and whileOff
                  type = 5;
                }
                else if (runningTimer.type.whileON){
                  // whileOn
                  type = 3;
                }
                else if (runningTimer.type.whileOFF){
                  // whileOff
                  type = 4;
                }
                else{
                  // just Interval
                  type = 2;
                } 
              }    
              else{
              }
              MBstart(my.Address);
              iicStr[2] = 4;          // Command Timer-Type
              iicStr[3] = i;          // ID of port
              iicStr[4] = type;       // type of port
              MBstop(5);

              switch (state){
              case 1:
                // perm. on (no time)
                break;
              case 2:
                // temp. off
              case 3:
                // temp. on
                MBstart(my.Address);
                iicStr[2] = 5;          // Command temporary end time
                iicStr[3] = i;          // ID of port
                MBaddLong(runningState[i].tempUntil, 4);
                MBstop(8);
                break;
              case 4:
                // automatic
                MBstart(my.Address);
                iicStr[2] = 6;          // Command lastAction and nextAction time
                iicStr[3] = i;          // ID of port
                MBaddLong(runningState[i].lastAction, 4);
                MBaddLong(runningState[i].nextAction, 8);
                MBstop(12);
              default:
                // perm. off (no time)
                break;
              }

            }
            digitalWrite(i + 2, runningState[i].state.lastVal);
            runningState[i].state.hasChanged = 0;
          }
          pos++; 
        }
        r = 1;
      }
    }
    
    // Print Runtime
    /*
    EscLocate(67,1);
    EscInverse(1);
    PrintRunTime();    
    // Print Realtime
    EscLocate(61,24);
    PrintDateTime();
    Serial.print(F(" "));
    //EscColor(0);
    EscInverse(0);    
    */
    if (!my.Boot){
      PrintLoopTimes();    
    }
    else if (my.Boot < 3){
      // ModBus RTU & AscII
    }
    else{
      // just values - send heart-beat
      MBstart(my.Address);
      iicStr[2] = 0;          // HeartBeat
      MBaddLong(myTime, 3);
      MBstop(7);
    }
    firstRun = 0;
  }

  pos = GetONEchar();
  if (pos){
    if (!my.Boot){
      if (pos > 96 && pos < 97 + RUNNING_TIMERS_CNT){
        PrintEditMenu(pos - 97);
        PrintLoopMenu();
      }
      else{
        PrintMainMenu();
      }
    }
    else{
      // We're not in Terminal-Mode
      // Force 1x values output
      firstRun = 1;
    }
  }
}