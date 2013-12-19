/*
      乐联第二期电量模块Arduino(TM)程序
 ================================
 by Jack Zhong (781890679@qq.com)
 
 ver：              0.9.0.0
 
 最后修改：         2013-12-17
 
 
 适用范围：         第二期电量模块；
 
 *****非电量模块的话，只支持每次上传一个数据，而不支持批量上传。*****
 
 本程序没有采用官方的leiweiClient网卡驱动，而是采用的更加精简的lwSimpleHTTPClient，
 更加适合mini+W5100/5500系列硬件
 
 */



// LeWei AC Power Meter trail success2013.06.25
// LeWei AC Power Meter (ZongBiao60A)trail syccess 2013.06.30 18:50pm
// 4 Parameter: watt / kwh / Amp / Voltage / Pf

//#include "tinyDHT.h"
#include <SPI.h>
#include <Ethernet.h>
#include "lwSimpleHTTPClient.h"

#define USERKEY          "029b3884b91e4d00b514158ba1e2ac57" // 使用你自己的USERKEY
#define LW_GATEWAY       "01"                               //网关
#define ACTIVE_SLAVE_ID  1                                  //电量模块中485的slaveid，默认为1
#define DHTPIN           3
#define QUERYINTERVAL    200
#define ERROR_NONE 0
#define ERROR_TIMEOUT 1
#define ERROR_CHECKSUM 2

lwSimpleHTTPClient *hClient;


void setup()
{
  Serial.begin(4800);
  hClient = new lwSimpleHTTPClient(USERKEY, LW_GATEWAY);
  //一直获取DHCP直到成功，也可以在获取DHCP失败之后，调用重载的initialize()，设置固定的IP
  while (!hClient->initialize())
  {
    delay(1000);
  }
}

/* Modbus para */
int tt[8];  //int changed to unsigned int //原来是30

byte readSensor(float &humidity, float &temperature)
{
    // Make sure we don't poll the sensor too often
    // - Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
    // - Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)
    unsigned long startTime = millis();

    // Request sample

    digitalWrite(DHTPIN, LOW); // Send start signal
    pinMode(DHTPIN, OUTPUT);
    //if ( model == DHT11 ) {
    delay(18);
    // }
    //else {
    // This will fail for a DHT11 - that's how we can detect such a device
    //  delayMicroseconds(800);
    //}

    pinMode(DHTPIN, INPUT);
    digitalWrite(DHTPIN, HIGH); // Switch bus to receive data

    // We're going to read 83 edges:
    // - First a FALLING, RISING, and FALLING edge for the start bit
    // - Then 40 bits: RISING and then a FALLING edge per bit
    // To keep our code simple, we accept any HIGH or LOW reading if it's max 85 usecs long

    word rawHumidity;
    word rawTemperature;
    word data;

    for ( int8_t i = -3 ; i < 2 * 40; i++ )
    {
        byte age;
        startTime = micros();

        do
        {
            age = (unsigned long)(micros() - startTime);
            if ( age > 90 )
            {
                //error = ERROR_TIMEOUT;
                return ERROR_TIMEOUT;
            }
        }
        while ( digitalRead(DHTPIN) == (i & 1) ? HIGH : LOW );

        if ( i >= 0 && (i & 1) )
        {
            // Now we are being fed our 40 bits
            data <<= 1;

            // A zero max 30 usecs, a one at least 68 usecs.
            if ( age > 30 )
            {
                data |= 1; // we got a one
            }
        }

        switch ( i )
        {
        case 31:
            rawHumidity = data;
            break;
        case 63:
            rawTemperature = data;
            data = 0;
            break;
        }
    }

    // Verify checksum

    if ( (byte)(((byte)rawHumidity) + (rawHumidity >> 8) + ((byte)rawTemperature) + (rawTemperature >> 8)) != data )
    {
        return ERROR_CHECKSUM;
    }

 
    humidity = rawHumidity >> 8;
    temperature = rawTemperature >> 8;

    return ERROR_NONE;
}

void loop()
{
  read_holding_registers(ACTIVE_SLAVE_ID, 0x49, 6, tt, 1); // 1:5,2:7,3:9

  int  Voltage  = tt[0] / 100;
  float  Amp = tt[1] ;
  Amp = Amp / 1000;
  int Watt = tt[2];

  float Kwh = ((unsigned long) tt[3] * 65536 + (unsigned int)tt[4]) / 3200;
  float Pf =  tt[5] ;
  Pf = Pf / 1000;

  float humidity = 0;
  float temperature = 0;

  if (  readSensor(humidity, temperature) != 0)
  {
    humidity = 0;
    temperature = 0;
  }

  // bool postBatchPowerInfo(int watt, float amp, float kwh, float pf, float voltage, float temperature=0.00, float humidity=0.00);
  hClient->postBatchPowerInfo(Watt, Amp, Kwh, Pf, Voltage, temperature, humidity);


  delay(15000);
}






// this method makes a HTTP connection to the server:


/****************************************************************************
 * BEGIN MODBUS RTU MASTER FUNCTIONS
 ****************************************************************************/

//#define TIMEOUT 1000          /* 1 second */
#define TIMEOUT 10000          /* 10 second */
#define MAX_READ_REGS 125
#define MAX_WRITE_REGS 125
#define MAX_RESPONSE_LENGTH 256
#define PRESET_QUERY_SIZE 256
/* errors */
#define PORT_ERROR -5

/*
CRC
 
 INPUTS:
 buf   ->  Array containing message to be sent to controller.
 start ->  Start of loop in crc counter, usually 0.
 cnt   ->  Amount of bytes in message being sent to controller/
 OUTPUTS:
 temp  ->  Returns crc byte for message.
 COMMENTS:
 This routine calculates the crc high and low byte of a message.
 Note that this crc is only used for Modbus, not Modbus+ etc.
 ****************************************************************************/

inline unsigned int crc(unsigned char *buf, int start, int cnt)
{
  int i, j;
  unsigned int temp, temp2, flag;

  temp = 0xFFFF;

  for (i = start; i < cnt; i++)
  {
    temp = temp ^ buf[i];

    for (j = 1; j <= 8; j++)
    {
      flag = temp & 0x0001;
      temp = temp >> 1;
      if (flag)
        temp = temp ^ 0xA001;
    }
  }

  /* Reverse byte order. */

  temp2 = temp >> 8;
  temp = (temp << 8) | temp2;
  temp &= 0xFFFF;

  return (temp);
}


/***********************************************************************
 *
 *      The following functions construct the required query into
 *      a modbus query packet.
 *
 ***********************************************************************/

#define REQUEST_QUERY_SIZE 6     /* the following packets require          */
#define CHECKSUM_SIZE 2          /* 6 unsigned chars for the packet plus   */
/* 2 for the checksum.                    */


void build_request_packet(int slave, int function, int start_addr,
int count, unsigned char *packet)
{
  packet[0] = slave;
  packet[1] = function;
  start_addr -= 1;
  packet[2] = start_addr >> 8;
  packet[3] = start_addr & 0x00ff;
  packet[4] = count >> 8;
  packet[5] = count & 0x00ff;
}

/*************************************************************************
 *
 * modbus_query( packet, length)
 *
 * Function to add a checksum to the end of a packet.
 * Please note that the packet array must be at least 2 fields longer than
 * string_length.
 **************************************************************************/

void modbus_query(unsigned char *packet, size_t string_length)
{
  int temp_crc;

  temp_crc = crc(packet, 0, string_length);

  packet[string_length++] = temp_crc >> 8;
  packet[string_length++] = temp_crc & 0x00FF;
  packet[string_length] = 0;
}



/***********************************************************************
 *
 * send_query(query_string, query_length )
 *
 * Function to send a query out to a modbus slave.
 ************************************************************************/

int send_query(unsigned char *query, size_t string_length)
{
  modbus_query(query, string_length);
  string_length += 2;

  for (int i = 0; i < string_length; i++)
  {
    Serial.write(query[i]); //JingLi
  }
  /* without the following delay, the reading of the response might be wrong
   * apparently, */
  delay(200);            /* FIXME: value to use? * 原来是200/
   
   return i;           /* it does not mean that the write was succesful, though */
}


/***********************************************************************
 *
 *      receive_response( array_for_data )
 *
 * Function to monitor for the reply from the modbus slave.
 * This function blocks for timeout seconds if there is no reply.
 *
 * Returns:     Total number of characters received.
 ***********************************************************************/

byte receive_response(unsigned char *received_string)
{

  byte bytes_received = 0;
  int i = 0;
  /* wait for a response; this will block! */
  while (Serial.available() == 0)
  {
    delay(1);
    if (i++ > TIMEOUT)
      return bytes_received;
  }
  delay(200);
  /* FIXME: does Serial.available wait 1.5T or 3.5T before exiting the loop? */
  while (Serial.available())
  {
    received_string[bytes_received] = Serial.read();
    bytes_received++;
    if (bytes_received >= MAX_RESPONSE_LENGTH)
      return PORT_ERROR;
  }

  return (bytes_received);
}


/*********************************************************************
 *
 *      modbus_response( response_data_array, query_array )
 *
 * Function to the correct response is returned and that the checksum
 * is correct.
 *
 * Returns:     string_length if OK
 *           0 if failed
 *           Less than 0 for exception errors
 *
 *      Note: All functions used for sending or receiving data via
 *            modbus return these return values.
 *
 **********************************************************************/

int modbus_response(unsigned char *data, unsigned char *query)
{
  byte response_length;
  int i;

  //为了减少程序运行时分配太多内存，屏蔽了下面变量已经后续的方法
  //  unsigned int crc_calc = 0;
  // unsigned int crc_received = 0;
  //unsigned char recv_crc_hi;
  //unsigned char recv_crc_lo;

  do          // repeat if unexpected slave replied
  {
    response_length = receive_response(data);
  }
  while ((response_length > 0) && (data[0] != query[0]));
  //      for (i = 0; i <response_length; i++) {           Serial.print(data[i]);Serial.print("---");   Serial.println(query[i]);}                       //only test

  if (response_length)
  {

//  *****************original calculation method, modified by Jack************************************
/*
    crc_calc = crc(data, 0, response_length - 2);

    recv_crc_hi = (unsigned) data[response_length - 2];
    recv_crc_lo = (unsigned) data[response_length - 1];

    crc_received = data[response_length - 2];
    crc_received = (unsigned) crc_received << 8;
    crc_received =
      crc_received | (unsigned) data[response_length - 1];
*/


    /*********** check CRC of response ************/

    if (crc(data, 0, response_length - 2) != (((unsigned)  data[response_length - 2] <<8)|(unsigned) data[response_length - 1]))
      // if (crc_calc != crc_received)
    {
      response_length = 0;
    }



    /********** check for exception response *****/

    if (response_length && data[1] != query[1])
    {
      response_length = 0 - data[2];
    }
  }
  return (response_length);
}


/************************************************************************
 *
 *      read_reg_response
 *
 *      reads the response data from a slave and puts the data into an
 *      array.
 *
 ************************************************************************/

int read_reg_response(int *dest, int dest_size, unsigned char *query)
{

  unsigned char data[MAX_RESPONSE_LENGTH];
  int raw_response_length;
  int temp, i;

  raw_response_length = modbus_response(data, query);
  if (raw_response_length > 0)
    raw_response_length -= 2;

  if (raw_response_length > 0)
  {
    /* FIXME: data[2] * 2 ???!!! data[2] isn't already the byte count (number of registers * 2)?! */
    for (i = 0;
                i < (data[2] * 2) && i < (raw_response_length / 2);
                i++)
    {

      /* shift reg hi_byte to temp */
      temp = data[3 + i * 2] << 8;
      /* OR with lo_byte           */
      temp = temp | data[4 + i * 2];

      dest[i] = temp;
    }
  }
  return (raw_response_length);
}


/***********************************************************************
 *
 *      preset_response
 *
 *      Gets the raw data from the input stream.
 *
 ***********************************************************************/

// int preset_response(unsigned char *query)
// {
//     unsigned char data[MAX_RESPONSE_LENGTH];
//     int raw_response_length;

//     raw_response_length = modbus_response(data, query);

//     return (raw_response_length);
// }


/************************************************************************
 *
 *      read_holding_registers
 *
 *      Read the holding registers in a slave and put the data into
 *      an array.
 *
 *************************************************************************/

int read_holding_registers(byte slave, byte start_addr, byte count,
int *dest, int dest_size)
{
  byte function = 0x03;      /* Function: Read Holding Registers */
  int ret;

  unsigned char packet[REQUEST_QUERY_SIZE + CHECKSUM_SIZE];

  if (count > MAX_READ_REGS)
  {
    count = MAX_READ_REGS;
  }

  switch (slave)
  {
  case 11: //奇怪的是，如果直接发送命令给电量模块，取到的数字为0，待查
    Serial.write(0x01);
    Serial.write(0x03);
    Serial.write(0x00);
    Serial.write(0x48);
    Serial.write(0x00);
    Serial.write(0x06);
    Serial.write(0x45);
    Serial.write(0xDE);
    ret = 0;
    delay(200);
    break;
  case 12:
    Serial.write(0x02);
    Serial.write(0x03);
    Serial.write(0x00);
    Serial.write(0x48);
    Serial.write(0x00);
    Serial.write(0x06);
    Serial.write(0x45);
    Serial.write(0xED);
    ret = 0;
    delay(200);
    break;
  default:
    build_request_packet(slave, function, start_addr, count, packet);
    ret = send_query(packet, REQUEST_QUERY_SIZE);
  }

  // build_request_packet(slave, function, start_addr, count, packet);
  // if (send_query(packet, REQUEST_QUERY_SIZE) > -1)
  if (ret >-1)
  {
    ret = read_reg_response(dest, dest_size, packet);
  }
  else
  {
    ret = -1;
  }

  return (ret);
}



