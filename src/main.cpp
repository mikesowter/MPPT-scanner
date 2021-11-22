/* 
Scans MPPT nano controllers to make data available to prometheus
monitors over-temperature and over-voltage conditions
logs major activity
cycles the nanos on and off by time of day calendar
I2C bus must be isolated from ground potential

Mike Sowter - November 2021 */

#include "main.h"
#include "functions.h"

void setup()
{
	bootMillis = millis();
	Serial.begin(115200);
	Serial.println("\n\rMPPT scanner 20211122");
  // setup watch dog
	secondTick.attach(1,ISRwatchDog);
	// join local network and internet
	joinNet();
  if ( network ) {
    // setup over the air updates
    init_OTA();
    // Set epoch and timers
    setupTime();
    // setup server responses
    setupServer();
    // setup FTP server
    ftpSrv.begin("mike","iron");
  }
  // setup file system and diag files
	setupFS();
  // setup as I2C master
	Wire.begin(SDA,SCL);
  scanBus();
	// lookup reason for restart
  resetReason.toCharArray(charBuf,resetReason.length()+1);
	errMess(charBuf);       // restart message
	resetDetail.toCharArray(charBuf,resetDetail.length()+1);
  if ( charBuf[16] != '0' )	errMess(charBuf);    
}

void loop()
{ 
  // scan all nanos 
  scan2Wire();
	// check for async activity
  watchWait(3000);
  // periodic processing
  if (oldMin != minute()) minProc();
  // check Prometheus scanning
  checkPromScan();
}

void ISRwatchDog () {
  noInterrupts();
  watchDog++; 
  if (watchDog >= 60) {
    errMess("watchDog 60s");
    fd.close();
		fe.close();
    ESP.restart();
  }
}

// this checks for network connectivity if no scan requests received
// if LAN was down at boot this will reconnect when possible

void checkPromScan() {
  if (  millis()-lastScan > 330000UL ) {
    diagMess("Prometheus 5m scan fail");
    // rejoin local network if necessary
    if (WiFi.status() != WL_CONNECTED) {
      if ( network ) errMess("\nnetwork disconnected\n");
      bool previouslyConnected = network;
      joinNet();        // attempt to rejoin network
    // if network now available, restart
      if ( network && !previouslyConnected) {    
        fd.close();
        fe.close();
        ESP.restart();
      }
    }
    lastScan = millis();
  }
}

void watchWait(uint32_t timer) {
  t0 = millis() + timer;
  while (millis() < t0) {   // wait for timeout
    yield();
    // reset watchdog
    watchDog = 0;
    if (network) {
      //  check for web requests
      server.handleClient();
      // check for OTA
      ArduinoOTA.handle();
      // check for FTP request
      ftpSrv.handleFTP(FS_ID);
    }
  }
}

