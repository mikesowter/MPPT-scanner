#include "functions.h"
#include <TimeLib.h>
#include <LittleFS.h>


extern WiFiClient client;
extern File fh,fd,fe;
extern volatile int watchDog;
extern bool onBattery;          // switch to enable sleep if required

extern char fileName[];
extern char fileSizeStr[];
extern char todayName[];
extern char userText[];
extern uint16_t longStrLen;
extern char longStr[];
extern char charBuf[];
extern char dateStr[];
extern char timeStr[];

extern uint8_t oldMin, oldFive, oldHour, oldDay, oldMonth;
extern float batteryVolts, amps[], amps0ref, refmax, refmin;
extern float dayCharge[], dayDischa[];
extern uint32_t fileSize;
extern uint32_t t0, t1, bootMillis, startMillis, lastScan;

uint8_t storeData();


void minProc() {
  oldMin = minute();
  // flush files
  fd.flush();
  fe.flush();
  fh.flush();
  // check for 5 minutes
  if ( oldFive == minute()/5 ) return;
  storeData();      // write day file every 5 mins
  oldFive = minute()/5;
  // check for end of day
  if (oldDay == day()) return;
  setupTime();
}  


