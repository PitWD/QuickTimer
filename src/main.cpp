#include "myMenu.h"

#define WIRE Wire

// Seconds between RTC sync's (0 disables sync)
#define syncRTCinterval 0

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  Wire.setClock(31000L);
  Wire.begin();

  RTC_GetDateTime();


memset(runningTimers, 0, RUNNING_TIMERS_CNT * sizeof(TimerSTRUCT));

/*
strcpy(runningTimers[0].name, "Light Flower-1");
runningTimers[0].type.interval = 1;
runningTimers[0].onTime = 30600L;
runningTimers[0].offTime = 27000L;
runningTimers[0].state.automatic = 1;
strcpy(runningTimers[1].name, "Light Flower-2");
runningTimers[1].type.interval = 1;
runningTimers[1].onTime = 30600L;
runningTimers[1].offTime = 27000L;
runningTimers[1].offset = 900L;
runningTimers[1].state.automatic = 1;
strcpy(runningTimers[2].name, "Light Flower-3");
runningTimers[2].type.interval = 1;
runningTimers[2].onTime = 30600L;
runningTimers[2].offTime = 27000L;
runningTimers[2].offset = 1800L;
runningTimers[2].state.automatic = 1;
strcpy(runningTimers[3].name, "Light Flower-4");
runningTimers[3].type.interval = 1;
runningTimers[3].onTime = 30600L;
runningTimers[3].offTime = 27000L;
runningTimers[3].offset = 2700L;
runningTimers[3].state.automatic = 1;

strcpy(runningTimers[4].name, "Light Vega-1");
runningTimers[4].type.interrupted = 1;
runningTimers[4].type.interval = 1;
runningTimers[4].onTime = 30L;
runningTimers[4].offTime = 30L;
runningTimers[4].onTime2 = 5L;
runningTimers[4].offTime2 = 5L;
runningTimers[4].state.automatic = 1;

strcpy(runningTimers[5].name, "Light Vega-?");
runningTimers[5].type.interval = 1;
runningTimers[5].onTime = 37800L;
runningTimers[5].offTime = 16200L;
runningTimers[5].offset = 60L;

strcpy(runningTimers[6].name, "Light Clone-1");
runningTimers[6].type.interval = 1;
runningTimers[6].onTime = 37800L;
runningTimers[6].offTime = 16200L;
runningTimers[6].offset = 120L;
runningTimers[6].state.automatic = 1;
strcpy(runningTimers[7].name, "Light Clone-2");
runningTimers[7].type.interval = 1;
runningTimers[7].onTime = 37800L;
runningTimers[7].offTime = 16200L;
runningTimers[7].offset = 1320L;
runningTimers[7].state.automatic = 1;
strcpy(runningTimers[8].name, "Light Clone-3");
runningTimers[8].type.interval = 1;
runningTimers[8].onTime = 37800L;
runningTimers[8].offTime = 16200L;
runningTimers[8].offset = 2520L;
runningTimers[8].state.automatic = 1;

strcpy(runningTimers[9].name, "Water Flower-1");
runningTimers[9].type.dayTimer = 1;
runningTimers[9].onTime = 39600L;
runningTimers[9].offTime = 56800L;
runningTimers[9].offset = 0;
runningTimers[9].state.automatic = 1;

strcpy(runningTimers[10].name, "Water Flower-2");
runningTimers[10].type.interval = 1;
runningTimers[10].onTime = 300L;
runningTimers[10].offTime = 6900L;
runningTimers[10].offset = 33L;
runningTimers[10].state.automatic = 1;

strcpy(runningTimers[11].name, "Water Vega-1");
runningTimers[11].type.interval = 1;
runningTimers[11].onTime = 5;
runningTimers[11].offTime = 5;
runningTimers[11].offset = 3;
runningTimers[11].state.automatic = 1;
*/


for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
  EEPROM.get(i * sizeof(TimerSTRUCT), runningTimers[i]);
}

PrintMainMenu();

}

void loop() {
// put your main code here, to run repeatedly:

  byte r = 1;
  byte pos = 3;

  if (DoTimer()){
    // A Second is over...

    if (RunTimers()){
      // A Port has changed
      pos = 5;
      for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
        // Check state  
        if (runningTimers[i].state.permOff){   
        }
        else if (runningTimers[i].state.permOn){  
        }
        else if (runningTimers[i].state.tempOff && myTime < runningTimers[i].tempUntil){   
        }
        else if (runningTimers[i].state.tempOn && myTime < runningTimers[i].tempUntil){ 
        }
        else if (runningTimers[i].state.automatic){
        }
        else{
          // Fully disabled
          r = 0;
        }
        if (r){
          // Port is valid
          if (runningTimers[i].state.hasChanged){
            // And has changed
            PrintTimerLine1(i, 9, pos);
          }
          pos++; 
        }
        r = 1;
      }
    }
    
    // Print Runtime
    EscLocate(66,1);
    EscInverse(1);
    PrintRunTime();    
    // Print Realtime
    EscLocate(61,24);
    PrintDateTime();
    Serial.print(F(" "));
    //EscColor(0);
    EscInverse(0);    
    
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