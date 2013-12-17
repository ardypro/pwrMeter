// LeWei AC Power Meter trail success2013.06.25
// LeWei AC Power Meter (ZongBiao60A)trail syccess 2013.06.30 18:50pm
// 4 Parameter: watt / kwh / Amp / Voltage / Pf

/*
      电量模块读写、发送至乐联电量模板程序

      修改：         Jack Zhong (781890679#qq.com)
      最后修改日期： 2013-12-16

      历史：
*/

#include <SPI.h>
#include <Ethernet.h>
//#include <LeweiClient.h>
#include "modbusrtu.h"

#define USERKEY          "029b3884b91e4d00b514158ba1e2ac57" // 改成你的USERKEY
#define LW_GATEWAY       "01"                               // 改为你的网关号
#define DEBUGGING

#define SOFTSERIAL

#ifdef SOFTSERIAL
#include "SoftwareSerial.h"

#endif

//LeWeiClient *lwc;


void setup()
{
    Serial.begin(4800);
    //lwc = new LeWeiClient(USERKEY, LW_GATEWAY);
}

//bool readData(int &watt, float &amp, float &kwh, float &pf, int &voltage)
void loop()
{
  //Serial.println("looping");
  
    //声明变量来保存电量数据
    int     Voltage  ;
    float   Amp    ;
    int     Watt   ;
    float Kwh ;
    float Pf ;
    float Cabon ;

    if (available())
    {
        if (readData(Watt, Amp, Kwh, Pf, Voltage))
        {

            Serial.println("Read data");
//#ifdef DEBUGGING
            Serial.print(Voltage);
            Serial.print(Amp);
            Serial.print(Watt);
            Serial.print(Kwh);
            Serial.print(Pf);
            Serial.print(Cabon);
//#endif

            // 4 Parameter: watt / kwh / Amp / Voltage / Pf

           //lwc->append("YDL", Kwh);
            //lwc->append("GL", Watt);
           // lwc->append("DL", Amp);
           // lwc->append("DY", Voltage);
            //lwc->append("GLYS", Pf);

           // lwc->send();
        }

    }

    delay(15000);
}








