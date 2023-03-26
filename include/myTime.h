
#ifndef MYTIME_H
#define MYTIME_H

#include "quicklib.h"
#include <EEPROM.h>

#define RUNNING_TIMERS_CNT 16

struct TimerSTRUCT{
  uint32_t onTime[3];
  uint32_t offTime[3];
  uint32_t offset[3];
  struct typeUNION{
    byte interval     :1;
    byte whileON      :1;
    byte whileOFF     :1;
    byte dayTimer     :1;
    byte invert       :1;
  }type;
  byte weekDays;
    // 0 = all
    // 2nd Bit = Sunday
    // 3rd Bit = Monday
    // 4th...
  struct stateUNION{
    byte automatic    :1;
    byte permOff      :1;
    byte permOn       :1;
    byte tempOn       :1;
    byte tempOff      :1;
    byte lastVal      :1;
    byte hasChanged   :1;
  }state;
  uint32_t realOffTime;
  char name[17];
}runningTimer;

struct TimerStateSTRUCT{
  struct stateUNION{
    byte automatic    :1;
    byte permOff      :1;
    byte permOn       :1;
    byte tempOn       :1;
    byte tempOff      :1;
    byte lastVal      :1;
    byte hasChanged   :1;
  }state;
  uint32_t tempUntil;
  uint32_t lastAction;
  uint32_t nextAction;
}runningState[RUNNING_TIMERS_CNT];


void TimerFromRomRam(byte timer, byte ram){
  // Load Timer
  EEPROM.get(timer * sizeof(TimerSTRUCT), runningTimer);
  if (ram){
    // copy temporay stuff from ram, too
    //runningTimer.state.permOff = runningState[timer].state.permOff;
    //runningTimer.state.permOn = runningState[timer].state.permOn;
    runningTimer.state.tempOff = runningState[timer].state.tempOff;
    runningTimer.state.tempOn = runningState[timer].state.tempOn;
    runningTimer.state.lastVal = runningState[timer].state.lastVal;
    runningTimer.state.hasChanged = runningState[timer].state.hasChanged;
    runningTimer.state.automatic = runningState[timer].state.automatic;
  }
}

void TimerToRomRam(byte timer, byte rom){
  // Move the temporary stuff to ram
  runningState[timer].state.permOff = runningTimer.state.permOff;
  runningState[timer].state.permOn = runningTimer.state.permOn;
  runningState[timer].state.tempOff = runningTimer.state.tempOff;
  runningState[timer].state.tempOn = runningTimer.state.tempOn;
  runningState[timer].state.lastVal = runningTimer.state.lastVal;
  runningState[timer].state.hasChanged = runningTimer.state.hasChanged;
  runningState[timer].state.automatic = runningTimer.state.automatic;
  if (rom){
    // Save the lasting stuff to eeprom, too...
    runningTimer.state.tempOff = 0;
    runningTimer.state.tempOn = 0;
    runningTimer.state.lastVal = 0;
    runningTimer.state.hasChanged = 0;
    EEPROM.put(timer * sizeof(TimerSTRUCT), runningTimer);
  }
}

byte IntervalTimer(uint32_t timerIN, uint32_t onTime, uint32_t offTime, uint32_t offset, uint32_t *start, uint32_t *stop, uint32_t *last, uint32_t *next){
    // Check if the current timerIN-position is within the "on" or off interval
    // calling functions have to take care that timerIN >= offset
    // Start/Stop/Next/Last are absolute

    // |              a TimerIN (whileON)                                           a timerIN (whileOFF)
    // |                     |                                                             |
    // |-Offset-|-----onTime-----|-offTime-|-----onTime-----|-offTime-|-----onTime-----|-offTime-|-----onTime-----|-offTime-|-~
    //          |---------interval---------|---------interval---------|---------interval---------|---------interval---------|                          
    //          |            |             |                          |                    |     |
    //        start          |           stop                       start                  |   stop
    //                  currentPos(ON)                                                currentPos(OFF)
    //          |                |                                                     |         |
    //        last             next                                                  last      next

    uint32_t interval = (onTime + offTime);
    uint32_t currentPos = 0;
    if (interval){
      // Div by 0 protection...
      currentPos = (timerIN - offset) % interval;
    }
    
    // parts of common Start/Stop/Next/Last calculations
    timerIN -= currentPos;
    offTime = timerIN + onTime;
    *start = timerIN;
    *stop = *start + interval;

    if (currentPos < onTime){
        // ON
        *next = offTime;
        *last = *start;
        return 1;
    }
    // OFF
    *next = *stop;
    *last = offTime;
    return 0;
}

/*  !!! DO NOT DELETE !!!!
byte CheckWhileTimer (byte whileO){
  // Check/Correct while-Timer Settings on validity
  // whileO ! 2 = whileOn
  // whileO = 2 = whileOff

  uint32_t onOff = runningTimer.onTime[0];

  if (whileO == 2){
    onOff = runningTimer.offTime[0];
  }
  else{
    whileO = 1;
  }
  
  if (runningTimer.offset[whileO] + runningTimer.onTime[whileO] + runningTimer.offTime[whileO] <= onOff){
    // All OK - regular case
    return 1;
  }
  else if (runningTimer.onTime[whileO] >= onOff){
    // onTime too long
    runningTimer.onTime[whileO] = onOff;
    runningTimer.offset[whileO] = 0;
    runningTimer.offTime[whileO] = 0;
  }
  else{
    if (runningTimer.onTime[whileO] + runningTimer.offset[whileO] >= onOff){
      // Offset too long
      runningTimer.offset[whileO] = onOff - runningTimer.onTime[whileO];
      runningTimer.offTime[whileO] = 0;
    }
    if (runningTimer.onTime[whileO] + runningTimer.offset[whileO] + runningTimer.offTime[whileO] >= onOff){
      // OffTime too long
      runningTimer.offTime[whileO] = onOff - runningTimer.offset[whileO] - runningTimer.onTime[whileO];
    }    
  }
  // Something has changed
  return 0;
}
*/

byte CalcIntervalTimer(uint32_t timerIN, byte timerID){

  byte r = 0;
  
  // 1st Level OnOff
  byte r1 = 0;
  uint32_t start1 = 0;
  uint32_t stop1 = 0;
  uint32_t last1 = 0;
  uint32_t next1 = 0;
  // 2nd Level OnOff
  byte r2 = 0;
  uint32_t start2 = 0;
  uint32_t stop2 = 0;
  uint32_t last2 = 0;
  uint32_t next2 = 0;

  r1 = IntervalTimer(timerIN, runningTimer.onTime[0], runningTimer.offTime[0], runningTimer.offset[0], &start1, &stop1, &last1, &next1);
  // 1st level 
  runningState[timerID].lastAction = last1;
  runningState[timerID].nextAction = next1;

  // Check if 1st interval is valid
  //if (IntervalTimer(timerIN, runningTimer.onTime[0], runningTimer.offTime[0], runningTimer.offset[0])){
  if (r1){
    r = 1;
    
    // Check if whileON Timer is active
    if (runningTimer.type.whileON){
      // Check if offset is expired
      if (timerIN > start1 + runningTimer.offset[1]){
        // Is 2nd Level ON or OFF
        r2 = IntervalTimer(timerIN - start1, runningTimer.onTime[1], runningTimer.offTime[1], runningTimer.offset[1], &start2, &stop2, &last2, &next2);
        // make result times absolute...
        // start2 += start1; stop2 += start1; last2 += start1; next2 += start1;
        last2 += start1; next2 += start1;

        // last Action
        runningState[timerID].lastAction = last2;
        // check if OnTime is active
        if (r2){
          // still ON
          if (next2 < next1){
            // full inside whileON
            runningState[timerID].nextAction = next2;
          }
          else{
            // touching or overlapping with whileOFF
            if (runningTimer.type.whileOFF){
              // whileOff exist
              if (runningTimer.offset[2]){
                // Offset exist
                runningState[timerID].nextAction = next1;
              }
              else{
                // without offset we stay full onTime On
                runningState[timerID].nextAction = next1 + runningTimer.onTime[2];
              }
            }
            else{
              // without whileOFF we're going Off at next1
              runningState[timerID].nextAction = next1;
            }
          }
        }
        else{
          // OffTime active
          r = 0;
          if (next2 < next1){
            // full inside whileON
            runningState[timerID].nextAction = next2;
          }
          else{
            // touching or overlapping with whileOFF
            if (runningTimer.type.whileOFF){
              // whileOff exist

              // we go On on next1
              runningState[timerID].nextAction = next1 + runningTimer.offset[2];
            }
            else{
              // without whileOFF we're at least Off until whileOff-time ends
              runningState[timerID].nextAction = stop1 + runningTimer.offset[1];
            }
          }
        }
      }
      else{
        // offset not expired
        r = 0;   
        runningState[timerID].nextAction = start1 + runningTimer.offset[1];           
      }
    }
    else if (runningTimer.type.whileOFF){
      // Never ON whileON - if just whileOFF exist
      r = 0;

      runningState[timerID].nextAction = next1 + runningTimer.offset[2];

      // To get last Action we need to go back in time (last second of whileOff)
      r2 = IntervalTimer(runningTimer.offTime[0] - 1, runningTimer.onTime[2], runningTimer.offTime[2], runningTimer.offset[2], &start2, &stop2, &last2, &next2);
      // make result times absolute and realtime...
      // start2 += start1 - runningTimer.offTime[0]; stop2 += start1 - runningTimer.offTime[0]; last2 += start1 - runningTimer.offTime[0]; next2 += start1 - runningTimer.offTime[0];
      

      if (r2){
        // ON, so went off at start1
        runningState[timerID].lastAction = start1;
      }
      else{
        // OFF
        runningState[timerID].lastAction = last2 + start1 - runningTimer.offTime[0];
      }
    }
  }

  else{
    // OFF

    // Check if WhileOff Timer is active
    if (runningTimer.type.whileOFF){
      // Check if offset is expired
      if (timerIN > last1 + runningTimer.offset[2]){
        // Is 2nd Level ON or OFF
        r2 = IntervalTimer(timerIN - last1, runningTimer.onTime[2], runningTimer.offTime[2], runningTimer.offset[2], &start2, &stop2, &last2, &next2);
        // make result times absolute...
        // start2 += last1; stop2 += last1; last2 += last1; next2 += last1;
        next2 += last1;

        // last Action
        runningState[timerID].lastAction = last2 + last1;
        
        // check if OnTime is active
        if (r2){
          // ON
          r = 1;

          if (next2 < stop1){
            // fully inside whileOFF
            runningState[timerID].nextAction = next2;
          }
          else{
            // touching or overlapping with whileON
            runningState[timerID].nextAction = stop1;
            if (runningTimer.type.whileON && !runningTimer.offset[1]){
              // whileON exist
              // without offset we stay full onTime1 On
              runningState[timerID].nextAction += runningTimer.onTime[1];
            }
          }
        }
        else{
          // still OFF
          if (next2 < stop1){
            // fully inside whileOFF
            runningState[timerID].nextAction = next2;
          }
          else{
            // touching or overlapping with whileON
            if (runningTimer.type.whileON){
              // whileON exist
              runningState[timerID].nextAction = stop1 + runningTimer.offset[1];
            }
            else{
              // without whileOn we go ON after whileON + offset[2]
              runningState[timerID].nextAction = stop1 + runningTimer.onTime[0] + runningTimer.offset[2];
            } 
          }
        }
      }
      else{
        // Offset not expired
        runningState[timerID].nextAction = last1 + runningTimer.offset[2]; 
      }
    }
    else if (runningTimer.type.whileON){
      // Never ON whileOFF - if just whileON exist

      runningState[timerID].nextAction = stop1 + runningTimer.offset[1];

      // To get last Action we need to go back in time (last second of whileON)
      r2 = IntervalTimer(runningTimer.onTime[0] - 1, runningTimer.onTime[1], runningTimer.offTime[1], runningTimer.offset[1], &start2, &stop2, &last2, &next2);
      // make result times absolute and realtime...
      // start2 += last1 - runningTimer.onTime[0]; stop2 += last1 - runningTimer.onTime[0]; last2 += last1 - runningTimer.onTime[0]; next2 += last1 - runningTimer.onTime[0];
      

      if (r2){
        // ON, so went off at last1
        runningState[timerID].lastAction = last1;
      }
      else{
        // OFF
        runningState[timerID].lastAction = last2 + last1 - runningTimer.onTime[0];
      }
    }      
  }  
  return r;
}

byte RunTimers(){
  // Returns true if one ore more states have changed

  byte r = 0; // Helper
  byte r2 = 0;

  for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){

    // Load Timer
    TimerFromRomRam(i, 1);
    
    /*
    if (runningTimer.type.dayTimer){
      // 24h DayTimer...
      r = DayTimer(myTime, runningTimer.onTime[0], runningTimer.offTime[0]);
    }
    else*/ if (runningTimer.type.interval || runningTimer.type.dayTimer){
      // Interval Timer(s)
      r = CalcIntervalTimer(myTime, i);
    }
    // Check on valid weekdays
    if (!getBit(runningTimer.weekDays, GetWeekDay(myTime)) && runningTimer.weekDays){
      // Day is not valid
      r = 0;
    }
    // Check on permanent & temporary state
    if (runningTimer.state.permOff){
      r = 0;
    }
    else if (runningTimer.state.permOn){
      r = 1;
    }
    else if (runningTimer.state.tempOff && myTime < runningState[i].tempUntil){
      r = 0;
    }
    else if (runningTimer.state.tempOn && myTime < runningState[i].tempUntil){
      r = 1;
    }
    else if (runningTimer.state.automatic){
      // r = r;
    }
    else{
      // Totally disabled
      r = 0;
    }
    // Invert if needed
    if (runningTimer.type.invert){
      r = !r;
    }
    // Check on 'regular' change
    if (r != runningTimer.state.lastVal){
      // Is changed
      runningTimer.state.lastVal = r;
      runningTimer.state.hasChanged = 1;
      r2 = 1;
    }
    else{
      // runningTimer.state.hasChanged = 0;
    }
    // Clear Temp-Times and States if possible
    if (runningState[i].tempUntil && (myTime >= runningState[i].tempUntil)){
      runningState[i].tempUntil = 0;
      runningTimer.state.tempOn = 0;
      runningTimer.state.tempOff = 0;
      runningTimer.state.hasChanged = 1;
      r2 = 1;
    }
    // Move the temporary stuff to ram
    TimerToRomRam(i, 0);
  }
  return r2;
}

#endif