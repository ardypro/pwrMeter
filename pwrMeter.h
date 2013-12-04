#ifndef PWRMETER_H
#define PWRMETER_H

/*
  ModbusReaderMethodTwo - example from LT/JSY C code

  ModbusReaderMethodTwo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ModbusReaderMethodTwo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with ModbusReaderMethodTwo.  If not, see <http://www.gnu.org/licenses/>.

  Edited by Lozoo 932871121<linzaiz@qq.com> on 2013.11.22
  Copyright © 2009-2013 Lozoo 932871121<linzaiz@qq.com>
*/

/*
    Modified by Jack Zhong (jzopen#yeah.net)

    HISTORY:
        2013-12-04  支持多表，默认的表ID=1，这也是电量表默认的ID
        2013-12-03  支持软串口


    Instruction:
    ------------

    pwrMeter meter;

    meter.begin(&Serial,4800);      //设置与485通信的串口以及波特率

    if (meter.available())         //检查是否有数据进入缓冲区
    {
        if (meter.readData(功率，电流，电量，功率因数，电压))
        {
            Serial.print(电压);
            ...
        }
        delay(2000);              //延时，等待下一个操作
    }
*/

//如果跟485通信的是软串口，就需要屏蔽下面这句
//#define HARDSERIAL

#ifdef HARDSERIAL
#include "HardwareSerial.h"
#else
#include "SoftwareSerial.h"
#endif

#define TX_BUFFER_SIZE 8
#define TIMEOUT 10000          /* 10 second */
#define MAX_RESPONSE_LENGTH 256
#define PORT_ERROR -5
#include "Arduino.h"

class pwrMeter
{
public:
    pwrMeter();
    virtual ~pwrMeter();
#ifdef HARDSERIAL
    void begin(HardwareSerial* serial, int baud=4800);
#else
    void begin(SoftwareSerial* serial, int baud=4800);
#endif
    int available(byte SlaveID=1);
    bool readData(int &watt, float &amp, float &kwh, float &pf, float &voltage);

protected:
    int send_query(unsigned char *query, size_t string_length);
    int receive_response(unsigned char *received_string);
    unsigned int calccrc(unsigned char crcbuf,unsigned int crc);
    unsigned int chkcrc(unsigned char *buf,unsigned char len);
    int Analysis_data(void);
    int read_data(void);
    byte Read_ID;
    union crcdata
    {
        unsigned int word16;
        unsigned char byte[2];
    };

    struct commdata
    {
        unsigned int Status;
        unsigned int nRx;
    }
    Comm1;
private:
#ifdef HARDSERIAL
    HardwareSerial* UART;
#else
    SoftwareSerial* UART;
#endif

    unsigned char Tx_Buffer[TX_BUFFER_SIZE];
    unsigned char RX_Buffer[MAX_RESPONSE_LENGTH];

    unsigned int     Voltage_data;
    unsigned int     Current_data;
    unsigned int     Power_data ;
    unsigned long    Energy_data;
    unsigned int     Pf_data;
};

#endif // PWRMETER_H
