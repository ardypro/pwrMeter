#include "lwSimpleHTTPClient.h"


lwSimpleHTTPClient::lwSimpleHTTPClient(const char* userkey, const char* gateway)
{
  userKey=userkey;
  gateWay=gateway;
}

lwSimpleHTTPClient::~lwSimpleHTTPClient()
{
  //dtor
}

bool lwSimpleHTTPClient::initialize()
{
  // TODO (Jack#1#): 以后增加支持固定IP、代理等
  byte mac[] = { 
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED  };

  //    if (Ethernet.begin(mac)==0)
  //        return false;
  //    else
  //        return true;

  return  Ethernet.begin(mac);
}

bool lwSimpleHTTPClient::initialize(IPAddress localip)
{
  byte mac[] = { 
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED  };
  Ethernet.begin(mac,localip);

  return true; //不管正确与否都返回true，可以尝试根据ethernet.localIP跟localip对比来判断
}

//#define post(l,v) ({sendHeader();client.println(lengthOfInt(l)+LENTH+strlen(sensor));client.println("Connection: close");client.println();client.print("[{\"Name\":\"");client.print(sensor);client.print("\",\"Value\":\"");client.print(v);client.println ("\"}]");})

void lwSimpleHTTPClient::sendHeader()
{
  //    Serial.println("sending json");

  client.print("POST /api/V1/Gateway/UpdateSensors/");
  client.print(gateWay);
  client.println(" HTTP/1.1");
  client.print("userkey: ");
  client.println(userKey);
  client.println("Host: open.lewei50.com");
  client.println("User-Agent: UNO");
  client.print("Content-Length: ");

#ifdef DEBUGGING
  Serial.print("POST /api/V1/Gateway/UpdateSensors/");
  Serial.print(gateWay);
  Serial.println(" HTTP/1.1");
  Serial.print("userkey: ");
  Serial.println(userKey);
  Serial.println("Host: open.lewei50.com");
  Serial.println("User-Agent: UNO");
  Serial.print("Content-Length: ");
#endif
}

//#define CLIENTPRINT "client.print(value);"

bool lwSimpleHTTPClient::post(const char* sensor, int value)
{
  // TODO (Jack#1#): 以后尝试复用浮点的append()

  bool ret;
  if (client.connect(LEWEISERVER,80))
  {
    sendHeader();
    client.println(lengthOfInt(value)+LENTH+strlen(sensor));
    client.println("Connection: close");
    client.println();
    client.print("[{\"Name\":\"");
    client.print(sensor);
    client.print("\",\"Value\":\"");
    client.print(value);
    client.println ("\"}]");

#ifdef DEBUGGING
    Serial.println(lengthOfInt(value)+LENTH+strlen(sensor));

    Serial.println("Connection: close");
    Serial.println(""); //必须的空白行
    Serial.print("[{\"Name\":\"");
    Serial.print(sensor);
    Serial.print("\",\"Value\":\"");
    Serial.print(value);
    Serial.println ("\"}]");
#endif // DEBUGGING
    ret= true;
  }
  else
    ret=false;

exitHere:
  client.stop();
  return ret;
}


bool lwSimpleHTTPClient::post(const char* sensor, double value,unsigned int digits)
{
  bool ret;
  if (client.connect(LEWEISERVER,80))
  {
    sendHeader();
    client.println(lengthOfInt((int)value)+LENTH+strlen(sensor)+digits);
    client.println("Connection: close");
    client.println();
    client.print("[{\"Name\":\"");
    client.print(sensor);
    client.print("\",\"Value\":\"");
    client.print(value,digits);
    client.println ("\"}]");

#ifdef DEBUGGING
    Serial.println(lengthOfInt((int)value)+LENTH+strlen(sensor)+digits);
    Serial.println("Connection: close");
    Serial.println(""); //必须的空白行
    Serial.print("[{\"Name\":\"");
    Serial.print(sensor);
    Serial.print("\",\"Value\":\"");
    Serial.print(value, digits);
    Serial.println ("\"}]");
#endif // DEBUGGING
    ret= true;
  }
  else
    ret=false;
exitHere:
  client.stop();
  return ret;
}

// byte lwSimpleHTTPClient::lengthOfInt(int value)
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
// byte lwSimpleHTTPClient::lengthOfDbl(double value, int digits=2)
//{
//    byte ret=1;
//    if (value<0)
//        ret ++;
//    ret +=  lengthOfInt(abs((int )value) )+ digits ;
//    return ret;
//}

