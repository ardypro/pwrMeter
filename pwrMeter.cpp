#include "pwrMeter.h"


pwrMeter::pwrMeter()
{
    //ctor
}

pwrMeter::~pwrMeter()
{
    //dtor
}

int pwrMeter::send_query(unsigned char *query, size_t string_length)
{
    int i;
    for(i = 0; i < string_length; i++)
    {
        UART->write(query[i]); //JingLi
    }
    /* without the following delay, the reading of the response might be wrong
     * apparently, */
    delay(200);            /* FIXME: value to use? */

    return i;           /* it does not mean that the write was succesful, though */
}

int pwrMeter::receive_response(unsigned char *received_string)
{
    int bytes_received = 0;
    int i = 0;

    /* wait for a response; this will block! */
    while(UART->available() == 0)
    {
        delay(1);
        if(i++ > TIMEOUT)
        {
            UART->println("receive timeout.");
#ifdef DEBUGGING
            Serial.println("receive timeout.");
#endif
        }
        return bytes_received;
    }
    delay(200);
    /* FIXME: does UART->available wait 1.5T or 3.5T before exiting the loop? */
    while(UART->available())
    {
        received_string[bytes_received] = UART->read();  //ORIGINAL.
        bytes_received++;
        if(bytes_received >= MAX_RESPONSE_LENGTH)
        {
            return PORT_ERROR;
        }
    }


    return (bytes_received);
}

unsigned int pwrMeter::calccrc(unsigned char crcbuf,unsigned int crc)
{
    unsigned char i;
    unsigned char chk;
    crc=crc ^ crcbuf;
    for(i=0; i<8; i++)
    {
        chk=(unsigned char)(crc&1);
        crc=crc>>1;
        crc=crc&0x7fff;
        if(chk==1)
        {
            crc=crc^0xa001;
        }
        crc=crc&0xffff;
    }
    return crc;
}

unsigned int pwrMeter::chkcrc(unsigned char *buf,unsigned char len)
{
    unsigned char hi,lo;
    unsigned int i;
    unsigned int crc;
    crc=0xFFFF;
    for(i=0; i<len; i++)
    {
        crc=calccrc(*buf,crc);
        buf++;
    }
    hi=(unsigned char)(crc%256);
    lo=(unsigned char)(crc/256);
    crc=(((unsigned int)(hi))<<8)|lo;
    return crc;
}

int pwrMeter::Analysis_data(void)
{
    int ret;
    unsigned char i;
    union crcdata
    {
        unsigned int word16;
        unsigned char byte[2];
    }
    crcnow;


    ret = -1;
    if(Comm1.Status==2)
    {
        if(RX_Buffer[0]==Read_ID)
        {
            crcnow.word16=chkcrc(RX_Buffer,Comm1.nRx-2); //Comm1.nRx
            if((crcnow.byte[0]==RX_Buffer[Comm1.nRx-1])&&(crcnow.byte[1]==RX_Buffer[Comm1.nRx-2])) //CRC
            {
                Voltage_data=(((unsigned int)(RX_Buffer[3]))<<8)|RX_Buffer[4]; //Voltage_data
                Current_data=(((unsigned int)(RX_Buffer[5]))<<8)|RX_Buffer[6]; //Current_data
                Power_data=(((unsigned int)(RX_Buffer[7]))<<8)|RX_Buffer[8]; //Power_data
                Energy_data=(((unsigned long)(RX_Buffer[9]))<<24)|(((unsigned long)(RX_Buffer[10]))<<16)|(((unsigned long)(RX_Buffer[11]))<<8)|RX_Buffer[12]; ////Energy_data
                Pf_data=(((unsigned int)(RX_Buffer[13]))<<8)|RX_Buffer[14]; //Pf_data
                ret=1;
            }

        }


        Comm1.Status=0;
    }

    return ret;
}

int pwrMeter::read_data(void)
{
    int ret;
    union crcdata
    {
        unsigned int word16;
        unsigned char byte[2];
    }
    crcnow;

    delay(1800);
    Tx_Buffer[0]=Read_ID;
    Tx_Buffer[1]=0x03;
    Tx_Buffer[2]=0x00;
    Tx_Buffer[3]=0x48;
    Tx_Buffer[4]=0x00;
    Tx_Buffer[5]=0x06;
    crcnow.word16=chkcrc(Tx_Buffer,6);
    Tx_Buffer[6]=crcnow.byte[1]; //CRC
    Tx_Buffer[7]=crcnow.byte[0];

    ret= send_query(Tx_Buffer, TX_BUFFER_SIZE);
#ifdef DEBUGGING
    Serial.println("has incoming data");
#endif
    return ret;
}

int pwrMeter::available()
{
    return read_data();
}

bool pwrMeter::readData(int &watt, float &amp, float &kwh, float &pf, float &voltage)
{
    int response_length = 0;
    response_length = receive_response(RX_Buffer);

    if(response_length>0)
    {
        Comm1.nRx = response_length;
        Comm1.Status = 2 ;
        if(Analysis_data() == 1)
        {
            voltage    = (float)Voltage_data / 100;
            amp        = (float)Current_data / 1000;
            watt       = Power_data;
            kwh        = (float)Energy_data/3200;
            pf         = (float)Pf_data / 1000;
        }
        return true ;
    }
    else
    {
        return false ;
    }
}

#ifdef HARDSERIAL
void pwrMeter::begin(HardwareSerial* serial, int baud)
{
    UART=serial;
    UART->begin(baud);
}
#else
void pwrMeter::begin(SoftwareSerial* serial, int baud)
{
    UART=serial;
    UART->begin(baud);
}
#endif


