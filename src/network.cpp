#include "secrets.h"
#include <arduino.h>
#include <TimeLib.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>

void sendNTPrequest(IPAddress& address);
unsigned long getNTPreply();
void diagMess(const char* mess);
extern bool network;

IPAddress localIP,ausTimeServerIP;
IPAddress localTimeServerIP(192, 168, 1, 6);
IPAddress ip(192, 168, 1, 63);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(192, 168, 1, 1);
uint32_t localPort = 2395, startSeconds; 

WiFiUDP udp;
WiFiClient client;

const uint8_t NTP_PACKET_SIZE = 48;
const uint8_t BUFFER_SIZE = 100;
uint8_t byteBuf[BUFFER_SIZE];
const char* ntpServerName = "au.pool.ntp.org";
const uint8_t TIME_ZONE = 10;
uint32_t timeOut;

void joinNet() {
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, pass);

  timeOut = millis() + 5000;
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    if ( millis() > timeOut ) {
      network = false;
      return;
    }
  }

  Serial.println("");
  Serial.println("local IP address: ");
  localIP=WiFi.localIP();
  Serial.print(localIP);
  long rssi = WiFi.RSSI();
  Serial.print("   signal strength: ");
  Serial.print(rssi);
  Serial.println(" dBm");

  udp.begin(localPort);
  // Resolve servers
  WiFi.hostByName(ntpServerName, ausTimeServerIP);
  network = true;
  yield();
}

unsigned long getTime() {

  unsigned long year2030 = 1893456000UL;
  unsigned long year2017 = 1483207200UL;

  uint32_t ms = millis();
  while (udp.parsePacket()!= NTP_PACKET_SIZE) {
    if (millis() - ms > 3500) {
      Serial.println("\nno reply from Aus NTP server");
      diagMess("no reply from Aus NTP server");
      break;
    }
    sendNTPrequest(ausTimeServerIP);
    delay(1000);
  }
  startSeconds = getNTPreply();
  if (startSeconds > year2017 && startSeconds < year2030) return startSeconds;

  while (udp.parsePacket()!= NTP_PACKET_SIZE) {
    if (millis() - ms > 1000) {
      diagMess("no reply from local NTP server");
      break;
    }
    sendNTPrequest(localTimeServerIP);
    delay(100);
  }
  startSeconds = getNTPreply();
  yield();
  if (startSeconds > year2017 && startSeconds < year2030) {
    setTime(startSeconds);
    diagMess("local NTP time");
    return startSeconds;
  }
  else diagMess("NTP time invalid");
return 0;
}

// send an NTP request to the time server at the given address
void sendNTPrequest(IPAddress& address)
{
  memset(byteBuf, 0, BUFFER_SIZE);
  // Initialize values needed to form NTP request
  byteBuf[0] = 0b11100011;   // LI, Version, Mode
  byteBuf[1] = 0;     // Stratum, or type of clock
  byteBuf[2] = 6;     // Polling Interval
  byteBuf[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  byteBuf[12]  = 49;
  byteBuf[13]  = 0x4E;
  byteBuf[14]  = 49;
  byteBuf[15]  = 52;
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(byteBuf, NTP_PACKET_SIZE);
  udp.endPacket();
}

unsigned long getNTPreply(){
  udp.read(byteBuf, NTP_PACKET_SIZE); // read the packet into the buffer
  unsigned long highWord = word(byteBuf[40], byteBuf[41]);
  unsigned long lowWord = word(byteBuf[42], byteBuf[43]);
  // this is NTP time :
  unsigned long secsSince1900 = highWord << 16 | lowWord;   // seconds since Jan 1 1900
  // now convert NTP time into Unix time:
  unsigned long GMT = secsSince1900 - 2208988800UL;         // seconds since Jan 1 1970
  //add timezone offset and return
  return GMT + TIME_ZONE*3600;
}
