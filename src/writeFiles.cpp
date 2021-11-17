#include <arduino.h>
#include <LittleFS.h>

void diagMess(const char* mess);
char* i2sd(uint8_t b);
char* f2s2(float f);
uint8_t openFile(const char* s);
char* timeStamp();

extern File fh;
extern char charBuf[],fileName[],todayName[];

uint8_t storeData() {
//  if ( !openFile("a") ) return 0;
//  fh.printf("%s,%.3f,%.3f\n",timeStamp());
//  fh.printf("%s,,%.2f,%.2f,%.2f,%.2f,",timeStamp(),amps[0],amps[1],amps[2],amps[3]);
//  fh.printf("Ah,%.2f,%.2f,%.2f\n",amphrs[1],amphrs[2],amphrs[3]);
//  fh.close(); 
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
