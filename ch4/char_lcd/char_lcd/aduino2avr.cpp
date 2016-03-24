/*
 * aduino2avr.c
 *
 * Created: 2016-03-06 오후 2:12:41
 *  Author: sela
 */ 
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include "aduino2avr.h"

void pinMode(uint8_t pin, uint8_t mode)
{
	if(pin > 13)
		return;
	
	if(pin > 7){
		pin -= 8;
		if(mode == INPUT)
			DDRB &= ~(1<<pin);
		else
			DDRB |= (1<<pin);
	}	
	else{
		if(mode == INPUT)
			DDRD &= ~(1<<pin);
		else
			DDRD |= (1<<pin);
	}
}

void digitalWrite(uint8_t pin, uint8_t val)
{
	if(pin > 13)
		return;
	
	if(pin > 7){
		pin -= 8;
		if(val == LOW)
			PORTB &= ~(1<<pin);
		else
			PORTB |= (1<<pin);
	}
	else{
		if(val == LOW)
			PORTD &= ~(1<<pin);
		else
			PORTD |= (1<<pin);
	}	
}

uint8_t digitalRead(uint8_t pin)
{
	uint8_t val;
	
	if(pin > 13)
		return 0;
	
	if(pin > 7){
		pin -= 8;
		if(PORTB & (1<<pin))
			val = HIGH;
		else
			val = LOW;
	}
	else{
		if(PORTD & (1<<pin))
			val = HIGH;
		else
			val = LOW;
	}
	return val;
}

void Print::printf(const char *format,...)
{
	char buf[32];
	size_t n = 0;
	
	va_list args;
	va_start(args, format);
	vsnprintf(buf, sizeof(buf)-1, format, args);

	while(1){
		uint8_t c = buf[n]; 
		if(c==0) break;
		if(write(c)) n++;
		else break;
	}
}