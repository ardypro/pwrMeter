#ifndef PWRMETER_H
#define PWRMETER_H

//如果跟485通信的是软串口，就需要屏蔽下面这句
//#define HARDSERIAL

#ifdef HARDSERIAL
#include "HardwareSerial.h"
#else
#include "SoftwareSerial.h"
#endif

#define TX_BUFFER_SIZE 8
#define Read_ID 1
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
    int available();
    bool readData(int &watt, float &amp, float &kwh, float &pf, float &voltage);
protected:
private:
#ifdef HARDSERIAL
    HardwareSerial* UART;
#else
    SoftwareSerial* UART;
#endif

    unsigned char Tx_Buffer[TX_BUFFER_SIZE];
    unsigned char RX_Buffer[MAX_RESPONSE_LENGTH ];

    struct commdata
    {
        unsigned int Status;
        unsigned int nRx;
    }
    Comm1;

    unsigned int     Voltage_data;
    unsigned int     Current_data;
    unsigned int     Power_data ;
    unsigned long    Energy_data;
    unsigned int     Pf_data;

    int send_query(unsigned char *query, size_t string_length);
    int receive_response(unsigned char *received_string);
    unsigned int calccrc(unsigned char crcbuf,unsigned int crc);
    unsigned int chkcrc(unsigned char *buf,unsigned char len);
    int Analysis_data(void);
    int read_data(void);

};

#endif // PWRMETER_H
