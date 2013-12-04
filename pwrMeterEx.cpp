#include "pwrMeterEx.h"

pwrMeterEx::pwrMeterEx()
{
    //ctor
}

pwrMeterEx::~pwrMeterEx()
{
    //dtor
}

bool pwrMeterEx::reset(byte slaveID)
{
//将有功总电能数据清零
//方法：将00 00 00 00写入000CH开始的2个寄存器，功能码为10H
//命令格式 01 10 00 0C 00 02 04 00 00 00 00 F3 FA (CRC)

    //Read_ID=slaveID;

    int ret;
    crcdata crcnow;

    delay(1800);

    unsigned char Buffer[13];

    Buffer[0]=slaveID;
    Buffer[1]=0x10;
    Buffer[2]=0x00;
    Buffer[3]=0x0C;
    Buffer[4]=0x00;
    Buffer[5]=0x02;
    Buffer[6]=0x04;
    Buffer[7]=0x00;
    Buffer[8]=0x00;
    Buffer[9]=0x00;
    Buffer[10]=0x00;


    crcnow.word16=chkcrc(Buffer,11);
    Buffer[11]=crcnow.byte[1]; //CRC
    Buffer[12]=crcnow.byte[0];

    ret= send_query(Buffer, 13);
#ifdef DEBUGGING
    Serial.println("has incoming data");
#endif
    return ret;
}

bool pwrMeterEx::changeSlaveID(byte slaveID, byte newID)
{
    /*
        读0004H的通讯设置

        命令：xx 03 00 04 00 01 xx xx (两个CRC)
        返回：xx 03 02 xx xx xx xx

    */
    unsigned char sendBuffer[8];
    unsigned char receiveBuffer[7];
    crcdata crcnow;

    sendBuffer[0]= slaveID;
    sendBuffer[1]=0x03;
    sendBuffer[2]=0x00;
    sendBuffer[3]=0x04;
    sendBuffer[4]=0x00;
    sendBuffer[5]=0x01;

    crcnow.word16=chkcrc(sendBuffer,6);
    sendBuffer[6]=crcnow.byte[1];
    sendBuffer[7]=crcnow.byte[0];

    int ret = send_query(sendBuffer,8);
//if
    Serial.print("query return\t");
    Serial.println(ret);
    Serial.println("received data");
    ret = receive_response(receiveBuffer);
    if(ret)
    {
        //有数据返回
        for(int i=0; i<8; i++)
        {
            Serial.println(receiveBuffer[i]);

        }

        crcnow.word16=chkcrc(receiveBuffer,5);
        Serial.println("CRC of data received");
        Serial.println(crcnow.byte[1]);
        Serial.println(crcnow.byte[0]);

        if((receiveBuffer[0]==slaveID)&& (receiveBuffer[5]==crcnow.byte[1]) && (receiveBuffer[6]==crcnow.byte[0]))
        {
            int high;
            if(newID<=247)
            {
                high=newID;
            }
            else
            {
                high=slaveID;
            }

            unsigned char Buffer[11];
            Buffer[0]=slaveID;
            Buffer[1]=0x10;
            Buffer[2]=0x00;
            Buffer[3]=0x04;
            Buffer[4]=0x00;
            Buffer[5]=0x01;
            Buffer[6]=0x02;
            Buffer[7]=high;
            Buffer[8]=receiveBuffer[4];

            crcnow.word16= chkcrc(Buffer,9);
            Buffer[9]=crcnow.byte[1];
            Buffer[10]=crcnow.byte[0];

            send_query(Buffer,11);

            return true ;
        }
    }
    else
        return false ;
}
