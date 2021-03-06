#include <arduino.h>
#include <wire.h>
#include <LittleFS.h>
#define FS_ID LittleFS
#define NUMNANO 1

void diagMess(const char* mess);
char* f2s2(float f);
char* f2s4(float f);
char* i2sh(uint8_t b);
char* timeStamp();
void ampHours(uint8_t bat);
uint8_t storeData();
void readI2Carray(uint8_t n);
void startMPPT(int n);
void stopMPPT(int n);

extern float avAmps[], amps[];
extern float Vimin[], Vimax[], Pmin[], Pavg[], Pmax[], Iout[], Vout[], Thi[], Tlo[], Vc[];
extern int nanoAddr[], numNanos;
extern bool mpptOn[], mpptOT[], mpptOV[];
extern uint32_t mpptOTSms[], mpptOVSms[];
extern char reply[];
extern uint8_t PWMmin[], PWMmax[];
extern uint16_t replycount;

float check;
uint8_t arrayPtr;
uint32_t timer;
uint16_t current;
char mess[100],buf[80];

void scan2Wire() {
  Serial.printf("\n%s ",timeStamp());
  for (uint8_t nano = 0; nano<numNanos ; nano++) {
    Wire.requestFrom(nanoAddr[nano], 32);      // request 32 bytes from slave device 
    strcpy(mess,"");
    delay(2);
    arrayPtr = 0;
    while ( Wire.available() ) {
      uint8_t c = Wire.read();    // receive a byte as character
      reply[arrayPtr++] = c;
    }
    readI2Carray(nano);
    // check temperature
    if ( !mpptOT[nano] && Thi[nano] > 70.0 ) {
      mpptOTSms[nano] = millis();
      mpptOT[nano] = true;
      sprintf(mess,"Over temperature in MPPT%d",nano);
      diagMess(mess);
      stopMPPT(nano);
    }
    else if ( mpptOT[nano] && Thi[nano] < 40.0 ) {
      mpptOT[nano] = false;
      sprintf(mess,"Over temperature cancelled in MPPT%d",nano);
      diagMess(mess);
      startMPPT(nano);
    }
    // check voltage into battery
    float Vbattery = Vout[nano] - 0.7*Iout[nano];
    if ( Vbattery > 14.2 ) {
      mpptOV[nano] = true;
      sprintf(mess,"Over 14.2V in MPPT%d",nano+1);
      diagMess(mess);
      stopMPPT(nano);
    }
    else if ( !mpptOV[nano] && Vbattery > 14.0 ) {
      mpptOVSms[nano] = millis();
      mpptOV[nano] = true;
      sprintf(mess,"Float started in MPPT%d",nano+1);
      diagMess(mess);
    } // only sit above 14V for one hour max
    else if ( mpptOV[nano] && (millis()-mpptOVSms[nano]>3600000) ) {
      mpptOV[nano] = false;
      sprintf(mess,"Float duration exceeded in MPPT%d",nano+1);
      diagMess(mess);
      stopMPPT(nano);
    }
    else if ( mpptOV[nano] && Vbattery < 13.5 ) {
      mpptOV[nano] = false;
      sprintf(mess,"Float cancelled in MPPT%d",nano+1);
      diagMess(mess);
    }
    Serial.printf("reply count: %d", replycount);
  }
}

float unpackflt() {
  float val;
  val = (float)reply[arrayPtr++];
  val += (float)reply[arrayPtr++]/100.0;
  return val;
}

void readI2Carray(uint8_t n) {
  arrayPtr = 0;
  Vimin[n] = unpackflt();
  Vimax[n] = unpackflt();
  Pavg[n] = unpackflt();
  Iout[n] = unpackflt();
  Vout[n] = unpackflt();
  Thi[n] = unpackflt();
  Tlo[n] = unpackflt();
  Vc[n] = unpackflt();
  PWMmin[n] = reply[arrayPtr++];  // reply[16]
  PWMmax[n] = reply[arrayPtr++];
  check = unpackflt();
  Pmin[n] = unpackflt();
  Pmax[n] = unpackflt();
  replycount = unpackflt();
  Serial.printf("\n%x Vin: %5.2f-%4.2f Iout: %3.2f Vout: %.2f Pavg: %.2f Thi: %.2f Tlo: %.2f ",
          n,Vimin[n],Vimax[n],Iout[n],Vout[n],Pavg[n],Thi[n],Tlo[n]);
} 