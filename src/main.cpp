#include "myMenu.h"

#define WIRE Wire

// Seconds between RTC sync's (0 disables sync)
#define syncRTCinterval 0

void setup() {
  // put your setup code here, to run once:

  Serial.begin(mySpeed);

  Wire.setClock(31000L);
  Wire.begin();

  RTC_GetDateTime();

// memset(runningTimers, 0, RUNNING_TIMERS_CNT * sizeof(TimerSTRUCT));
EscCls();
for (byte i = 0; i < RUNNING_TIMERS_CNT; i++){
  EEPROM.get(i * sizeof(TimerSTRUCT), runningTimers[i]);
  // Check on validity
  if (strlen(runningTimers[i].name) == 0 || strlen(runningTimers[i].name) > 16){
    // Undefined / New
    Serial.print(F("Undefined Port/Timer : "));
    Serial.println(i + 1);
    memset(&runningTimers[i], 0, sizeof(TimerSTRUCT));
    strcpy(runningTimers[i].name, "Default-");
    runningTimers[i].name[8] = 'A' + i; 
    runningTimers[i].name[9] = 0;
    EEPROM.put(i * sizeof(TimerSTRUCT), runningTimers[i]); 
  }
  // Set Output & StartValue
  pinMode(i + 2, OUTPUT);
  if (runningTimers[i].type.invert){
    digitalWrite(i + 2, HIGH);
  }
  else{
    digitalWrite(i + 2, LOW);
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
            digitalWrite(i + 2, runningTimers[i].state.lastVal);
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