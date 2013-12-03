#include <Arduino.h>

#include "pwrMeter.h"
#include "SoftwareSerial.h"

pwrMeter meter;

// 读取电量数据之前，必须先声明用来保存电量数据的各个变量
int Watt=0;
float Amp=0;
float Kwh=0;
float Pf=0;
float Voltage=0;

SoftwareSerial mySerial(10, 11); // RX, TX, 本例使用软串口跟485通信
                                 // 使用软串口跟485通信的话，需要屏蔽pwrMeter.h中的宏定义#define HARDSERIAL
                                 // 注意不是所有的引脚都支持软串口，定义前需要查看资料

void setup()
{
    Serial.begin(9600);
    meter.begin(&mySerial,4800); // 设置与485通信的串口以及波特率

    //或者按下列方式初始化
    //mySerial.begin(4800);
    //meter.begin(&mySerial);
}

void loop()
{

    if(meter.available()>0)     // 有数据可供读取
    {
        if(meter.readData(Watt,Amp,Kwh,Pf,Voltage)) // 一次性读入所有的电量数据
        {
            Serial.println();
            Serial.print("Voltage: \t");
            Serial.println(Voltage);
            Serial.print("Amp: \t");
            Serial.println(Amp);
            Serial.print("Watt: \t");
            Serial.println(Watt);
            Serial.print("Kwh: \t");
            Serial.println(Kwh);
            Serial.print("Pf: \t");
            Serial.println(Pf);
            Serial.println();
        }
    }

    delay(2000);

}

