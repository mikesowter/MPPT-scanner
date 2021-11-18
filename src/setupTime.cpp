#include <arduino.h>
#include <TimeLib.h>

#define resetRainMonth 2 // D4

char* dateStamp();
char* timeStamp();
unsigned long getTime();

extern uint8_t oldMin,oldFive,oldHour,oldDay,oldMonth;
extern uint32_t t0, startMillis;
extern char todayName[], charBuf[];

void setupTime() {
  setTime(getTime());
  //setTime(23,59,30,30,11,2016);
  startMillis = millis();
  oldMin = minute();
  oldFive = oldMin/5;
  oldHour = hour();
  oldDay = day();
  oldMonth = month();
  strcpy(todayName,"MS");
  strcat(todayName,dateStamp());
  strcat(todayName,".csv");
}

