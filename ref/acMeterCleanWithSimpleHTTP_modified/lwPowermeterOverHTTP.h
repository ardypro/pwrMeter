#ifndef LWPOWERMETEROVERHTTP_H
#define LWPOWERMETEROVERHTTP_H

#include "lwSimpleHTTPClient.h"
#include "Ethernet.h"
#include "SPI.h"
#include "Arduino.h"

#define DEBUGGING


//inline byte lengthOfInt(int value)
//{
//    if (value>0)
//        {
//            return (int)log10(value)+1;
//        }
//    else if (value==0)
//        return 1;
//    else
//        return (int) log10((abs(value)))+2;
//}
//
//inline byte lengthOfDbl(double value, int digits=2)
//{
//    byte ret=1;
//    if (value<0)
//        ret ++;
//    ret +=  lengthOfInt(abs((int )value) )+ digits ;
//    return ret;
//}

class lwPowermeterOverHTTP : 
public lwSimpleHTTPClient
{
public:
  lwPowermeterOverHTTP(const char* userkey, const char* gateway);
  virtual ~lwPowermeterOverHTTP();
  bool postBatchPowerInfo(int watt, float amp, float kwh, float pf, int voltage, float temperature=0.00, float humidity=0.00);
 
protected:
private:
};

#endif // LWPOWERMETEROVERHTTP_H

