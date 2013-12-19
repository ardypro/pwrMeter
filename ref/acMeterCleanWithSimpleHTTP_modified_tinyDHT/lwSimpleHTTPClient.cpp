#include "lwSimpleHTTPClient.h"


lwSimpleHTTPClient::lwSimpleHTTPClient(const char *userkey, const char *gateway)
{
    userKey = userkey;
    gateWay = gateway;
}

lwSimpleHTTPClient::~lwSimpleHTTPClient()
{
    //dtor
}

bool lwSimpleHTTPClient::initialize()
{
    // TODO (Jack#1#): 以后增加支持固定IP、代理等
    byte mac[] =
    {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
    };

    //    if (Ethernet.begin(mac)==0)
    //        return false;
    //    else
    //        return true;

    return  Ethernet.begin(mac);
}

bool lwSimpleHTTPClient::initialize(IPAddress localip)
{
    byte mac[] =
    {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
    };
    Ethernet.begin(mac, localip);

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
    client.print("Host: ");
    client.println(LEWEISERVER);
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
/*
bool lwSimpleHTTPClient::post(const char *sensor, int value)
{
    // TODO (Jack#1#): 以后尝试复用浮点的append()

    bool ret;
    if (client.connect(LEWEISERVER, 80))
    {
        sendHeader();
        client.println(lengthOfInt(value) + LENTH + strlen(sensor));
        client.println("Connection: close");
        client.println();

        sendSensorname(sensor);

        //client.print("[{\"Name\":\"");
        //client.print(sensor);
        //client.print("\",\"Value\":\"");
        client.print(value);
        client.println ("\"}]");

#ifdef DEBUGGING
        Serial.println(lengthOfInt(value) + LENTH + strlen(sensor));

        Serial.println("Connection: close");
        Serial.println(""); //必须的空白行
        Serial.print("[{\"Name\":\"");
        Serial.print(sensor);
        Serial.print("\",\"Value\":\"");
        Serial.print(value);
        Serial.println ("\"}]");
#endif // DEBUGGING
        ret = true;
    }
    else
        ret = false;

exitHere:
    client.stop();
    return ret;
}


bool lwSimpleHTTPClient::post(const char *sensor, double value, unsigned int digits)
{
    bool ret;
    if (client.connect(LEWEISERVER, 80))
    {
        sendHeader();
        client.println(lengthOfInt((int)value) + LENTH + strlen(sensor) + digits); //?小数点位
        client.println("Connection: close");
        client.println();
        sendSensorname(sensor);

        //client.print("[{\"Name\":\"");
        //client.print(sensor);
        //client.print("\",\"Value\":\"");
        client.print(value, digits);
        client.println ("\"}]");

#ifdef DEBUGGING
        Serial.println(lengthOfInt((int)value) + LENTH + strlen(sensor) + digits);
        Serial.println("Connection: close");
        Serial.println(""); //必须的空白行


        Serial.print("[{\"Name\":\"");
        Serial.print(sensor);
        Serial.print("\",\"Value\":\"");
        Serial.print(value, digits);
        Serial.println ("\"}]");
#endif // DEBUGGING
        ret = true;
    }
    else
        ret = false;
exitHere:
    client.stop();
    return ret;
}
*/
void lwSimpleHTTPClient::sendSensorname(const char *sensor)
{
    client.print("{\"Name\":\"");
    client.print(sensor);
    client.print("\",\"Value\":\"");
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


bool lwSimpleHTTPClient::postBatchPowerInfo(int watt, float amp, float kwh, float pf, int voltage, float temperature, float humidity)
{
    bool ret;


    if (client.connect(LEWEISERVER, 80))
    {

        byte len = 179; //去除value值之后的所有字符长度

        len += lengthOfInt(watt);
        len += lengthOfDbl(amp);
        len += lengthOfDbl(kwh);
        len += lengthOfDbl(pf);
        len += lengthOfInt(voltage);
        len += lengthOfDbl(temperature);
        len += lengthOfDbl(humidity);

        sendHeader();
        client.println(len);
        client.println("Connection: close");
        client.println();

        client.print("[");
        //功率
        sendSensorname("GL");
        //client.print("{\"Name\":\"GL");
        //client.print("\",\"Value\":\"");
        client.print(watt);
        client.print("\"},");

        //电流
        sendSensorname("DL");
        //client.print("{\"Name\":\"DL");
        //client.print("\",\"Value\":\"");
        client.print(amp);
        client.print("\"},");

        //用电量
        sendSensorname("YDL");
        //client.print("{\"Name\":\"YDL");
        //client.print("\",\"Value\":\"");
        client.print(kwh);
        client.print("\"},");

        //功率因数
        sendSensorname("GLYS");
        //client.print("{\"Name\":\"GLYS");
        //client.print("\",\"Value\":\"");
        client.print(pf);
        client.print("\"},");

        //电压
        sendSensorname("DY");
        //client.print("{\"Name\":\"DY");
        //client.print("\",\"Value\":\"");
        client.print(voltage);
        client.print("\"},");

        //温度
        sendSensorname("WD");
        //client.print("{\"Name\":\"WD");
        //client.print("\",\"Value\":\"");
        client.print(temperature);
        client.print("\"},");

        //湿度
        sendSensorname("SD");
        //client.print("{\"Name\":\"SD");
        //client.print("\",\"Value\":\"");
        client.print(humidity);
        client.print("\"}");

        client.println ("]");
        client.println();

#ifdef DEBUGGING
        Serial.println(len);
        Serial.println("Connection: close");
        Serial.println();

        Serial.print("[");
        //功率
        Serial.print("{\"Name\":\"GL");
        Serial.print("\",\"Value\":\"");
        Serial.print(watt);
        Serial.print("\"},");

        //电流
        Serial.print("{\"Name\":\"DL");
        Serial.print("\",\"Value\":\"");
        Serial.print(amp);
        Serial.print("\"},");

        //用电量
        Serial.print("{\"Name\":\"YDL");
        Serial.print("\",\"Value\":\"");
        Serial.print(kwh);
        Serial.print("\"},");

        //功率因数
        Serial.print("{\"Name\":\"GLYS");
        Serial.print("\",\"Value\":\"");
        Serial.print(pf);
        Serial.print("\"},");

        //电压
        Serial.print("{\"Name\":\"DY");
        Serial.print("\",\"Value\":\"");
        Serial.print(voltage);
        Serial.print("\"},");

        //温度
        Serial.print("{\"Name\":\"WD");
        Serial.print("\",\"Value\":\"");
        Serial.print(temperature);
        Serial.print("\"},");

        //湿度
        Serial.print("{\"Name\":\"SD");
        Serial.print("\",\"Value\":\"");
        Serial.print(humidity);
        Serial.print("\"}");

        Serial.println ("]");
        Serial.println();
        Serial.println();

#endif // DEBUGGING
        ret = true;
    }
    else
        ret = false;

exitHere:
    client.stop();
    return ret;
}
