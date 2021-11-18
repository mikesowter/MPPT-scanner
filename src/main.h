#include <arduino.h>
#include <wire.h>
#include <ADS1X15.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESPFTPServer.h>
#include <LittleFS.h>
#define FS_ID LittleFS
#include <Ticker.h>
#include <ArduinoOTA.h>
extern "C" {
#include "user_interface.h"
}
// mini pinout
#define LED 16 // D0
#define SDA 05 // D2
#define SCL 04 // D1

String resetReason = "restart: " + ESP.getResetReason();
String resetDetail = ESP.getResetInfo();

ESP8266WebServer server(80);
FtpServer ftpSrv;
time_t getNtpTime();
FSInfo fs_info;
File fh,fd,fe;
Ticker secondTick;
volatile int watchDog = 0;

bool onBattery = false;      
bool charging = true;    
bool reboot = true;
bool network = false;

char fileName[] = "/XXyy-mm-dd.csv";
char fileSizeStr[10];
char todayName[] = "/XXyy-mm-dd.csv";
char userText[] = "/XXyy-mm-dd.csv";
uint16_t longStrLen = 10000;
char longStr[10000];
char charBuf[128];
char dateStr[10];
char timeStr[10];

uint8_t oldMin, oldFive, oldHour, oldDay, oldMonth, PWMmin[8], PWMmax[8];
int nanoAddr[8], numNanos;
bool mpptOn[8], mpptOT[8], mpptOV[8];
uint32_t mpptOTSms[8], mpptOVSms[8];
char reply[33];
float Vimin[8], Vimax[8], Pmin[8], Pavg[8], Pmax[8], Iout[8], Vout[8], Thi[8], Tlo[8], Vc[8];

uint32_t fileSize;
uint32_t t0, t1, lastSamp, bootMillis, startMillis, lastScan;
