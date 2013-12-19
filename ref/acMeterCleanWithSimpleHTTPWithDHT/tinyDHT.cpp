/******************************************************************
  DHT Temperature & Humidity Sensor library for Arduino.

  Features:
  - Support for DHT11 and DHT22/AM2302/RHT03
  - Auto detect sensor model
  - Very low memory footprint
  - Very small code

  http://www.github.com/markruys/arduino-DHT

  Written by Mark Ruys, mark@paracas.nl.

  BSD license, check license.txt for more information.
  All text above must be included in any redistribution.

  Datasheets:
  - http://www.micro4you.com/files/sensor/DHT11.pdf
  - http://www.adafruit.com/datasheets/DHT22.pdf
  - http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Weather/RHT03.pdf
  - http://meteobox.tk/files/AM2302.pdf

  Changelog:
   2013-06-10: Initial version
   2013-06-12: Refactored code
   2013-07-01: Add a resetTimer method
 ******************************************************************/

#include "tinyDHT.h"

void tinyDHT::setup(uint8_t pin/*, DHT_MODEL_t model*/)
{
    tinyDHT::pin = pin;
    //tinyDHT::model = model;
    tinyDHT::resetTimer(); // Make sure we do read the sensor in the next readSensor()

    // if ( model == AUTO_DETECT) {
    //   tinyDHT::model = DHT22;
    //   readSensor();
    //   if ( error == ERROR_TIMEOUT ) {
    //     tinyDHT::model = DHT11;
    //     // Warning: in case we auto detect a DHT11, you should wait at least 1000 msec
    //     // before your first read request. Otherwise you will get a time out error.
    //   }

   // this->model = DHT11;

}

void tinyDHT::resetTimer()
{
    lastReadTime = millis() - 3000;
}

// float tinyDHT::getHumidity()
// {
//   readSensor();
//   return humidity;
// }

// float tinyDHT::getTemperature()
// {
//   readSensor();
//   return temperature;
// }
/*
#ifndef OPTIMIZE_SRAM_SIZE

const char *tinyDHT::getStatusString()
{
    switch ( error )
    {
    case tinyDHT::ERROR_TIMEOUT:
        return "TIMEOUT";

    case tinyDHT::ERROR_CHECKSUM:
        return "CHECKSUM";

    default:
        return "OK";
    }
}

#else

// At the expense of 26 bytes of extra PROGMEM, we save 11 bytes of
// SRAM by using the following method:


prog_char P_OK[]       PROGMEM = "OK";
prog_char P_TIMEOUT[]  PROGMEM = "TIMEOUT";
prog_char P_CHECKSUM[] PROGMEM = "CHECKSUM";

const char *tinyDHT::getStatusString()
{
    prog_char *c;
    switch ( error )
    {
    case tinyDHT::ERROR_CHECKSUM:
        c = P_CHECKSUM; break;

    case tinyDHT::ERROR_TIMEOUT:
        c = P_TIMEOUT; break;

    default:
        c = P_OK; break;
    }

    static char buffer[9];
    strcpy_P(buffer, c);

    return buffer;
}

#endif
*/

byte tinyDHT::readSensor(float &humidity, float &temperature)
{
    // Make sure we don't poll the sensor too often
    // - Max sample rate DHT11 is 1 Hz   (duty cicle 1000 ms)
    // - Max sample rate DHT22 is 0.5 Hz (duty cicle 2000 ms)
    unsigned long startTime = millis();

    // Request sample

    digitalWrite(pin, LOW); // Send start signal
    pinMode(pin, OUTPUT);
    //if ( model == DHT11 ) {
    delay(18);
    // }
    //else {
    // This will fail for a DHT11 - that's how we can detect such a device
    //  delayMicroseconds(800);
    //}

    pinMode(pin, INPUT);
    digitalWrite(pin, HIGH); // Switch bus to receive data

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
        while ( digitalRead(pin) == (i & 1) ? HIGH : LOW );

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
