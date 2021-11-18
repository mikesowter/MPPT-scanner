#include "functions.h"
#include <TimeLib.h>
#include <LittleFS.h>

extern WiFiClient client;
extern File fh,fd,fe;
extern volatile int watchDog;
extern bool onBattery;          // switch to enable sleep if required
extern bool mpptOn[], mpptOT[], mpptOV[];

extern char fileName[];
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
extern uint32_t t0, t1, bootMillis, startMillis, lastScan;

uint8_t storeData();
uint16_t solBegin[12] = {636,650,658,714,739,801,759,731,654,626,614,620};
uint16_t solEnd[12] = {1736,1722,1712,1658,1633,1611,1613,1641,1718,1746,1758,1752};

void minProc() {
  oldMin = minute();
  uint16_t hourMin = 100*hour() + minute();
  for (uint8_t nano = 0; nano<1; nano++ ) {
    if ( mpptOn[nano] && hourMin < solBegin[month()-1]) stopMPPT(nano);
    else if ( !mpptOn[nano] && hourMin >= solBegin[month()-1]) startMPPT(nano);
    else if (  mpptOn[nano] && hourMin >= solEnd[month()-1]) stopMPPT(nano);
  }
  // flush files
  fd.flush();
  fe.flush();
  // check for 5 minutes
  if ( oldFive == minute()/5 ) return;
  if ( mpptOn[0] ) storeData();      // write day file every 5 mins
  oldFive = minute()/5;
  // check for end of day
  if (oldDay == day()) return;
  setupTime();
}  


