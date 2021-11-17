#include <arduino.h>
#include <wire.h>
#include <functions.h>
#define XON 17
#define XOFF 19

void startMPPT(int n) {
  Wire.beginTransmission(n); // transmit to device addr n
  Wire.write(XON);           // sends one byte
  Wire.endTransmission();    // stop transmitting
}

void stopMPPT(int n) {
  Wire.beginTransmission(n); // transmit to device addr n
  Wire.write(XOFF);          // sends one byte
  Wire.endTransmission();    // stop transmitting
}

