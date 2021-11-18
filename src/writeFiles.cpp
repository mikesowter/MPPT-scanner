#include <arduino.h>
#include <LittleFS.h>

void diagMess(const char* mess);
char* i2sd(uint8_t b);
char* f2s2(float f);
uint8_t openFile(const char* s);
char* timeStamp();

extern File fh;
extern char charBuf[],todayName[];
extern int nanoAddr[], numNanos;
extern float Vimin[], Vimax[], Pmin[], Pavg[], Pmax[], Iout[], Vout[], Thi[], Tlo[];

uint8_t storeData() {
  if ( !openFile("a") ) return 0;
  fh.printf("%s,",timeStamp());
  for (int n=0; n<numNanos; n++) {
    fh.printf(",%d, %.2f, %.2f, %.2f, %.2f,",n,Vimin[n],Pavg[n],Vout[n],Thi[n]);
  }
  fh.printf("\n");
  fh.close(); 
  return 1; 
}

//----------------- open file 

uint8_t openFile(const char* s) {
  fh = LittleFS.open(todayName, s);
  if (!fh) {
    strcpy(charBuf,todayName);
    strcat(charBuf," failed to open");
    diagMess(charBuf);
    return 0;
  }
  return 1;
}
