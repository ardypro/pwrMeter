#ifndef PWRMETEREX_H
#define PWRMETEREX_H

#include <pwrMeter.h>


class pwrMeterEx : public pwrMeter
{
public:
    pwrMeterEx();
    virtual ~pwrMeterEx();

    bool reset(byte slaveID=1); //清零有功总电能数据
    bool changeSlaveID(byte slaveID, byte newID); //修改设备ID
protected:
private:
};

#endif // PWRMETEREX_H
