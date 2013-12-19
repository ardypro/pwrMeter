/*
 * DH11.cpp
 *
 *  Created on: 2012. 12. 12.
 *      Author: dalxx

 	Modified by Jack Zhong
 */

#include "simpleDHT11.h"
#include <Arduino.h>


simpleDHT11::simpleDHT11(byte pin_number) {
	this->pin=pin_number;
	this->last_read_time=0;
	pinMode(pin,INPUT);
	digitalWrite(pin, HIGH);

}
//wait for target status
//parameters
//  target : target status waiting for
//  time_out_us : time out in microsecond.
unsigned long simpleDHT11::waitFor(uint8_t target, unsigned long time_out_us) {
	unsigned long start=micros();
	unsigned long time_out=start+time_out_us;
	while(digitalRead(this->pin)!=target)
	{
		if(time_out<micros()) return -1;
	}
	return micros()-start;
}

//wait for target status.  //这地方容易假死
void simpleDHT11::waitFor(uint8_t target) {
	//放假死
	unsigned long t;
	t=micros();
	while ((digitalRead(this->pin)!=target) && ((micros()-t)<1000))
	{
		
	}
}

//read one bye
byte simpleDHT11::readByte() {
	int i=0;
	byte ret=0;
	for(i=7;i>=0;i--)
	{
		waitFor(HIGH); //wait for 50us in LOW status
		delayMicroseconds(30); //wait for 30us
		if(digitalRead(this->pin)==HIGH) //if HIGH status lasts for 30us, the bit is 1;
		{
			ret|=1<<(i);
			waitFor(LOW); //wait for rest time in HIGH status.
		}
	}
	return ret;
}

simpleDHT11::~simpleDHT11() {
	// TODO Auto-generated destructor stub
}
//parameters
//	temperature : temperature to read.
//	humidity : humidity to read.
//return -1 : read too shortly. retry latter .
//		  0 : read successfully
//        1 : DHT11 not ready.
//		  4 : Checksum Error
byte simpleDHT11::read(float& humidity, float& temperature) {
	if((millis()-this->last_read_time<DHT11_RETRY_DELAY)&&this->last_read_time!=0)	return -1;

	pinMode(pin,OUTPUT);
	digitalWrite(pin, LOW);
	delay(18);
	digitalWrite(pin, HIGH);
	pinMode(pin,INPUT);

	if(waitFor(LOW, 40)<0)	return 1; //waiting for DH11 ready
	if(waitFor(HIGH, 90)<0)	return 1; //waiting for first LOW signal(80us)
	if(waitFor(LOW, 90)<0)	return 1; //waiting for first HIGH signal(80us)

	byte hI=this->readByte();
	byte hF=this->readByte();
	byte tI=this->readByte();
	byte tF=this->readByte();
	byte cksum=this->readByte();
	if(hI+hF+tI+tF!=cksum)
		return 4;


	humidity=(float)hI+(((float)hF)/100.0F);
	temperature=(float)tI+(((float)tF)/100.0F);
	this->last_read_time=millis();
	return 0;
}


