#include <WiFiClient.h>

// main
void ISRwatchDog ();
void checkBattery();
void watchWait(uint32_t timer);
void checkPromScan();
void initAmps();
// energy
void ampHours(uint8_t bat);
// network
void joinNet();
unsigned long getTime();
void sendNTPrequest(IPAddress& address);
unsigned long getNTPreply();
// OTA
uint8_t init_OTA();
// diags
void diagMess(const char* mess);
void errMess(const char* mess);
char* dateStamp();
char* timeStamp();
void printHex(uint8_t X);
char* i2sh(uint8_t b);
char* i2sd(uint8_t b);
char* i3sd(uint8_t b);
char* f2s2(float f);
char* f2s4(float f);
// setupTime
void setupTime();
void showTime();
// updateTotal
uint8_t readTotal();
uint8_t updateTotal();
// dirList
void handleDir();
void listFile();
void helpPage();
// web
void handleMetrics();
void handleNotFound();
void addCstring(const char* s);
// minProc
void minProc();
// setupServer
void setupServer();
// setupFS
void setupFS ();
// writeFiles
uint8_t storeData();
uint8_t openFile(const char* s);
void writeTemps();
void readTemps();
void scan2Wire();


