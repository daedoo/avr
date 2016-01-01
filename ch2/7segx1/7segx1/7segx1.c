/*
 * 7segx1.c
 *
 * Created: 2015-12-12 오후 6:06:45
 * Author : sela
 */ 

// ATmega328P
#include <avr/io.h>
#include <avr/interrupt.h>
//#define F_CPU     8000000L
//#include <util/delay.h>

volatile uint32_t g_tcount = 0;
void time_delay(uint32_t us);

//              0  1  2  3  4  5  6  7  8  9  A  b  C  d  E  F
//PB7 - 1 (E)   1  0  1  0  0  0  1  0  1  0  1  1  1  1  1  1
//PB6 - 2 (D)   1  0  1  1  0  1  1  0  1  0  0  1  1  1  1  0
//PB5 - 4 (C)   1  1  0  1  1  1  1  1  1  1  1  1  0  1  0  0
//PB4 - 5 (DP)  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
//PB3 - 10(G)   0  0  1  1  1  1  1  0  1  1  1  1  0  1  1  1
//PB2 - 9 (F)   1  0  0  0  1  1  1  1  1  1  1  1  1  0  1  1
//PB1 - 7 (A)   1  0  1  1  0  1  1  1  1  1  1  0  1  0  1  1
//PB0 - 6 (B)   1  1  1  1  1  0  0  1  1  1  1  0  0  1  0  0
//             E7 21 CB 6B 2D 6E EE 27 EF 2F AF EC C6 E9 CE 8E

//              0  1  2  3  4  5  6  7  8  9  A  b  C  d  E  F
//PB7 - 7 (A)   1  0  1  1  0  1  1  1  1  1  1  0  1  0  1  1
//PB6 - 9 (F)   1  0  0  0  1  1  1  1  1  1  1  1  1  0  1  1
//PB5 - 10(G)   0  0  1  1  1  1  1  0  1  1  1  1  0  1  1  1
//PB4 - 1 (E)   1  0  1  0  0  0  1  0  1  0  1  1  1  1  1  1
//PB3 - 2 (D)   1  0  1  1  0  1  1  0  1  0  0  1  1  1  1  0
//PB2 - 4 (C)   1  1  0  1  1  1  1  1  1  1  1  1  0  1  0  0
//PB1 - 5 (DP)  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
//PB0 - 6 (B)   1  1  1  1  1  0  0  1  1  1  1  0  0  1  0  0
//             DD 05 B9 AD 65 EC FC C5 FD E5 F5 7C D8 3D F8 F0


uint8_t num[] =
{
	0xDD, 0x05, 0xB9, 0xAD, 0x65, 0xEC, 0xFC, 0xC5, 
	0xFD, 0xE5, 0xF5, 0x7C, 0xD8, 0x3D, 0xF8, 0xF0
};

int main(void)
{
	uint8_t value = 0;
	
	DDRB = 0xff;
	PORTB = num[0];
	
	// CTC mode
	TCCR0A = (1<<WGM01);
	// clk source, 8MHz/8
	TCCR0B = (1<<CS01);
	
	// output compare value for 100us
	OCR0A = 100;
	
	sei();
	
	while (1)
	{
		PORTB = num[value];
		// delay 1sec
		time_delay(10000);
		
		value += 1;
		if(value > 15)
		value = 0;
	}
}

ISR (TIMER0_COMPA_vect)
{
	g_tcount++;
}

void time_delay(uint32_t delay)
{
	g_tcount = 0;

	// timer/counter0 output compare match A interrupt enable
	TIMSK0 |= (1<<OCIE0A);
	
	while(1)
	{
		if(g_tcount >= delay){
			// disable interrupt
			TIMSK0 &= ~(1<<OCIE0A);;
			return;
		}
	}
}
