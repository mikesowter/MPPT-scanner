#include <arduino.h>

void diagMess(const char* mess);

extern bool onBattery ,charging;   
extern float avAmps[] ,amps[] ,amphrs[];  
extern float dayCharge[], dayDischa[];
extern uint32_t lastSamp; 
uint32_t ms, duration;

#define deadband 0.025

// accumulate Amphours for each battery between changes in direction of current flow.
// current flow is defined as positive if charging battery

void ampHours(uint8_t bat) {
  if ( bat == 0 ) {
    ms = millis();
    duration = ms > lastSamp ? ms - lastSamp : 1000;
    lastSamp = ms;
    if ( amps[bat] > 4.0 * deadband ) {
      if ( !charging ) {  // change to charging
        charging = true;
        diagMess("charging");
        for (uint8_t b=1;b<3;b++) {
          Serial.printf("battery %i discharge = %.2fAh\n",b,amphrs[b]/1000.0);
          dayDischa[b] += amphrs[b]/1000.0;
          amphrs[b] = 0.0;
        }
      } 
    }
    else {
      if ( amps[bat] < -4.0 * deadband ) {
        if ( charging && bat==1 ) {  // change to discharging
          charging = false;
          diagMess("discharging");
          for (uint8_t b=1;b<4;b++) {
            Serial.printf("battery %i charge = %.2fAh\n",b,amphrs[b]/1000.0);
            dayCharge[b] += amphrs[b]/1000.0;
            amphrs[b] = 0.0;
          }
        }
      }
    }
  }
  if ( amps[bat] > deadband || amps[bat] < -deadband ) {
    amphrs[bat] += float(duration)*amps[bat]/3600000.0;  //in amp hours
  }
}