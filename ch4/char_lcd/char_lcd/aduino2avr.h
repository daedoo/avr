/*
 * aduino2avr.h
 *
 * Created: 2016-03-06 ¿ÀÈÄ 2:12:41
 *  Author: sela
 */ 

#ifndef aduino2avr_h
#define aduino2avr_h
#include <stdio.h>
#include <util/delay.h>

#define delayMicroseconds(x) _delay_us(x)

#define INPUT			0
#define OUTPUT			1
#define INPUT_PULLUP	2

#define HIGH			1
#define LOW				0

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
uint8_t digitalRead(uint8_t pin);

class Print{
	public:
		virtual size_t write(uint8_t) = 0;
		void printf(const char *format, ...);
};

#endif