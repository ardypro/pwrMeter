/*
 * DH11.h
 *
 *  Created on: 2012. 12. 12.
 *      Author: dalxx
 *      Version : 0.8
 */

#ifndef SIMPLEDHT11_H_
#define SIMPLEDHT11_H_
#include <Arduino.h>
#define DHT11_RETRY_DELAY 1000  // 1000ms

class simpleDHT11 {
	byte pin;
	unsigned long last_read_time;
protected:

	byte readByte();
	unsigned long waitFor(uint8_t target, unsigned long time_out_us);
	void waitFor(uint8_t target);
public:
	simpleDHT11(byte pin_number);
	~simpleDHT11();
	byte read( float& humidity, float& temperature);



};


#endif /* simpleDHT11_H_ */
