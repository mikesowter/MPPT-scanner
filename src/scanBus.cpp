#include <arduino.h>
#include <wire.h>

void diagMess(const char* mess);
extern int numNanos, nanoAddr[];

void scanBus()
{
  byte error, address;
  numNanos = 0;
  for(address = 1; address < 127; address++ ) {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0) {
      Serial.print("I2C device at address 0x");
      if (address<16) Serial.print("0");
      Serial.println(address,HEX);
      nanoAddr[numNanos++] = address;
    }
    else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address<16) Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (numNanos == 0) diagMess(" no nanos found on bus\n");
  else if (numNanos > 8) diagMess(" >8 nanos found on bus\n");
}