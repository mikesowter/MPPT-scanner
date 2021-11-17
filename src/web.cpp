#include <arduino.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include "functions.h"

extern char longStr[], fileName[], userText[], charBuf[];
extern ESP8266WebServer server;
extern uint32_t lastScan, bootMillis;
extern File fd, fe, fh;
extern uint16_t longStrLen;
extern bool onBattery;
extern float amps[], amphrs[], ref1sec, refmax, refmin;
extern float Vimin[], Vimax[], Pmin[], Pavg[], Pmax[], Iout[], Vout[], Thi[], Tlo[], Vc[];
extern float dayCharge[], dayDischa[];
extern uint8_t reply[];

void gotoSleep();

void handleMetrics() {
  longStr[0] = '\0';
  //Vimin, Vimax, Pmin, Pavg, Iout, Vout, Thi, Tlo, Vc, PWMmin, PWMmax;

  addCstring("\n# TYPE MS1Vimin guage" );
  addCstring("\nMS1Vimin ");
  addCstring(f2s2(Vimin[0]));
  addCstring("\n# TYPE MS1Vimax guage" );
  addCstring("\nMS1Vimax ");
  addCstring(f2s2(Vimax[0]));
  addCstring("\n# TYPE MS1Pmin guage" );
  addCstring("\nMS1Pmin ");
  addCstring(f2s2(Pmin[0]));
  addCstring("\n# TYPE MS1Pavg guage" );
  addCstring("\nMS1Pavg ");
  addCstring(f2s2(Pavg[0]));
  addCstring("\n# TYPE MS1Pmax guage" );
  addCstring("\nMS1Pmax ");
  addCstring(f2s2(Pmax[0]));
  addCstring("\n# TYPE MS1Iout guage" );
  addCstring("\nMS1Iout ");
  addCstring(f2s2(Iout[0]));
  addCstring("\n# TYPE MS1Vout guage" );
  addCstring("\nMS1Vout ");
  addCstring(f2s2(Vout[0]));
  addCstring("\n# TYPE MS1Thi guage" );
  addCstring("\nMS1Thi ");
  addCstring(f2s2(Thi[0]));
  addCstring("\n# TYPE MS1Tlo guage" );
  addCstring("\nMS1Tlo ");
  addCstring(f2s2(Tlo[0]));

  addCstring("\n# TYPE MSWifiSignal guage" );
  addCstring("\nMSWifiSignal ");
  addCstring(f2s2(-WiFi.RSSI()));
  addCstring( "\n" );
  server.send ( 200, "text/plain", longStr );
  lastScan = millis();
}

void handleNotFound() {
  server.uri().toCharArray(userText, 14);

  if (LittleFS.exists(userText)) {
    listFile();
  }
  else if (strncmp(userText,"/favicon.ico",12)==0) {
  }
  else if (strncmp(userText,"/apple",6)==0) {
  }
  else if (strncmp(userText,"/deldiags",9)==0) {
    fd.close();
    LittleFS.remove("diags.txt");
    fd = LittleFS.open("diags.txt", "a");
    diagMess("diags deleted");
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Diagnostics deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
  else if (strncmp(userText,"/delerrs",8)==0) {
    fe.close();
    LittleFS.remove("errmess.txt");
    fe = LittleFS.open("errmess.txt", "a");
    errMess("errors cleared");
    fe.flush();
    strcpy(charBuf,"<!DOCTYPE html><html><head><HR>Error Messages deleted<HR></head></html>");
    server.send ( 200, "text/html", charBuf );
  }
    else if (strncmp(userText,"/reset",6)==0) {
    diagMess("reset requested");
    fd.close();
		fe.close();
    ESP.restart();
  }
    else {
    Serial.print(timeStamp());
    Serial.print(userText);
    Serial.println(" is not a valid option");
  }
}

void addCstring(const char* s) {
  // find end of longStr
  uint16_t p;
  for (p=0;p<longStrLen;p++) {
    if ( p>longStrLen-32) {
      diagMess("longStrLen exceeded");
      break;
    }
    if (longStr[p]=='\0') {
      break;    // p now points to end of old string
    }
  }
  uint16_t q=0;
  for (;p<longStrLen;p++) {
    longStr[p]=s[q];
    if (s[q++]=='\0') break;
  }
}
