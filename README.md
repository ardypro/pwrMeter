pwrMeter
========

485电量模块的Arduino类库 

根据麻瓜的示例程序整理而来，核心代码没做改动。在原来基础之上新增支持软串口功能，可以将有限的硬串口留给其它设备。

注意：如果使用软串口跟电量模块的485通信的话，需要屏蔽pwrMeter.h中的宏编译指令:
      #define HARDSERIAL
      


使用方法：

pwrMeter meter;

meter.begin(&Serial,4800);

if (meter.available()) //检查是否有数据进入缓冲区
{
    if (meter.readData(功率，电流，电量，功率因数，电压))
    {
      Serial.print(电压);
      ...
    }
    delay(2000); //延时，等待下一个操作
}




本软件的使用遵从免费、开源协议。 

Ardypro (jzopen#yeah.net)

2013-12-03
