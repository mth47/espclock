#include "CNTPClient.h"
#include "GlobalsAndDefines.h"

//NTP functions copied from the TimeNTP_ESP8266WiFi example of the Time library.

unsigned int localPort = 8888;  // local port to listen for UDP packets

CNTPClient::CNTPClient()
{
  
}

CNTPClient::~CNTPClient()
{
  
}

bool CNTPClient::setup(IPAddress timeServer)
{
  setTimeServer(timeServer);
  serialTrace.Log(T_INFO, "CNTPClient::Setup() - Starting UDP");
  Udp.begin(localPort);
  serialTrace.Log(T_INFO, "CNTPClient::Setup() - Local port: '%d'", Udp.localPort());
}

void CNTPClient::setTimeServer(IPAddress timeServer)
{
  m_timeServer = timeServer;
}

time_t CNTPClient::now()
{
  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  serialTrace.Log(T_DEBUG, "CNTPClient::now() - Transmit NTP Request");
  sendNTPpacket(m_timeServer);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) 
    {
      serialTrace.Log(T_DEBUG, "CNTPClient::now() - Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL;
    }
  }
  serialTrace.Log(T_ERROR, "CNTPClient::now() - No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void CNTPClient::sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:                 
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

