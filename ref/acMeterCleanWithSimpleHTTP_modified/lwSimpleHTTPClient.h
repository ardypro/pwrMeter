
#ifndef LWSIMPLEHTTPCLIENT_H
#define LWSIMPLEHTTPCLIENT_H


#include "Ethernet.h"
#include "SPI.h"
#include "Arduino.h"

#define LEWEISERVER "open.lewei50.com"
#define LENTH 25

#define DEBUGGING

inline byte lengthOfInt(int value)
{
  if (value>0)
  {
    return (int)log10(value)+1;
  }
  else if (value==0)
    return 1;
  else
    return (int) log10(abs(value))+2;
}

inline byte lengthOfDbl(double value, int digits=2)
{
  byte ret=1;
  if (value<0)
    ret ++;
  ret +=  lengthOfInt(abs((int )value) )+ digits ;
  return ret;
}


class lwSimpleHTTPClient
{
public:
  lwSimpleHTTPClient(const char* userkey, const char* gateway);
  virtual ~lwSimpleHTTPClient();
  bool initialize();
  bool initialize(IPAddress localip);
  bool post(const char* sensor, int value);
  bool post(const char* sensor, double value,unsigned int digits=2);
protected:
  void sendHeader();
  EthernetClient client;
private:
  const char* userKey;
  const char* gateWay;

};

#endif // LWSIMPLEHTTPCLIENT_H

