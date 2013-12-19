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

/*
    迷你版DHT11 Arduino类库

    修改：     Jack Zhong

    注意：     虽然上面原作者的声明中提到支持DHT22等，但是tinyDHT已经删除了对DHT11之外其它传感器的支持，如果您使用的是DHT22等，请使用原作者的版本

*/

#ifndef tinyDHT_h
#define tinyDHT_h

#include "Arduino.h"
    
#define ERROR_NONE 0
#define ERROR_TIMEOUT 1
#define ERROR_CHECKSUM 2

class tinyDHT
{
public:

    // typedef enum
    // {
    //     AUTO_DETECT,
    //     DHT11,
    //     DHT22,
    //     AM2302,  // Packaged DHT22
    //     RHT03    // Equivalent to DHT22
    // }
    // DHT_MODEL_t;



    // typedef enum
    // {
    //     ERROR_NONE = 0,
    //     ERROR_TIMEOUT,
    //     ERROR_CHECKSUM
    // }
    // DHT_ERROR_t;

    void setup(uint8_t pin/*, DHT_MODEL_t model=AUTO_DETECT*/);
    void resetTimer();
    byte readSensor(float &humidity, float &temperature);
    //float getTemperature();
    //float getHumidity();

    // DHT_ERROR_t getStatus() { return error; };
    // const char* getStatusString();

    // DHT_MODEL_t getModel() { return model; }

    // int getMinimumSamplingPeriod() { return model == DHT11 ? 1000 : 2000; }

    // int8_t getNumberOfDecimalsTemperature() { return model == DHT11 ? 0 : 1; };
    // int8_t getLowerBoundTemperature() { return model == DHT11 ? 0 : -40; };
    // int8_t getUpperBoundTemperature() { return model == DHT11 ? 50 : 125; };

    // int8_t getNumberOfDecimalsHumidity() { return 0; };
    // int8_t getLowerBoundHumidity() { return model == DHT11 ? 20 : 0; };
    // int8_t getUpperBoundHumidity() { return model == DHT11 ? 90 : 100; };

    static float toFahrenheit(float fromCelcius)
    {
        return 1.8 * fromCelcius + 32.0;
    };
    static float toCelsius(float fromFahrenheit)
    {
        return (fromFahrenheit - 32.0) / 1.8;
    };

protected:


    //float temperature;
    //float humidity;

    uint8_t pin;

private:
    //DHT_MODEL_t model;
    //DHT_ERROR_t error;
    unsigned long lastReadTime;
};

#endif /*dht_h*/
