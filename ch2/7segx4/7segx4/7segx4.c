/*
 * 7segx4.c
 *
 * Created: 2015-12-11 오후 9:25:16
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
//#define F_CPU     8000000L
//#include <util/delay.h>

//#define DECIMAL_MODE
#define HEXADECIMAL_MODE

//#define CC_MODE
#define CA_MODE

volatile uint32_t g_tcount = 0;
uint16_t value = 0;

void time_delay(uint32_t us);

//              0  1  2  3  4  5  6  7  8  9  A  b  C  d  E  F
//PB7 -10 (F)   1  0  0  0  1  1  1  1  1  1  1  1  1  0  1  1
//PB6 -11 (A)   1  0  1  1  0  1  1  1  1  1  1  0  1  0  1  1
//PB5 - 1 (E)   1  0  1  0  0  0  1  0  1  0  1  1  1  1  1  1
//PB4 - 2 (D)   1  0  1  1  0  1  1  0  1  0  0  1  1  1  1  0
//PB3 - 3 (DP)  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0
//PB2 - 4 (C)   1  1  0  1  1  1  1  1  1  1  1  1  0  1  0  0
//PB1 - 5 (G)   0  0  1  1  1  1  1  0  1  1  1  1  0  1  1  1
//PB0 - 7 (B)   1  1  1  1  1  0  0  1  1  1  1  0  0  1  0  0
//             F5 05 73 57 87 D6 F6 C5 F7 C7 E7 B6 F0 37 F2 E2

uint8_t num[] =
{
	0xF5, 0x05, 0x73, 0x57, 0x87, 0xD6, 0xF6, 0xC5,
	0xF7, 0xC7, 0xE7, 0xB6, 0xF0, 0x37, 0xF2, 0xE2
};

#ifdef CC_MODE
uint8_t seg[] =
{
	(1<<PD3), (1<<PD4), (1<<PD5), (1<<PD7)
};
#else
uint8_t seg[] =
{
	~(1<<PD3)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
	~(1<<PD4)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
	~(1<<PD5)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
	~(1<<PD7)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
};
#endif

int main(void)
{
	DDRB = 0xff;
	DDRD = (1<<PD3)|(1<<PD4)|(1<<PD7)|(1<<PD5);
	PORTB = num[0];
	
	// CTC mode
	TCCR0A = (1<<WGM01);
	// clk source, 8MHz/8 = 500kHz
	TCCR0B = (1<<CS01);
	
	// output compare value for 100us
	OCR0A = 100;  // 10kHz
	
	sei();
	
	while (1)
	{
		// delay 1sec
		time_delay(10000);
		value += 1;
#ifdef DECIMAL_MODE
		if(value > 10000)
			value = 0;
#endif
	}
}

ISR (TIMER0_COMPA_vect)
{
	static uint8_t count = 0;
	static uint8_t index = 0;
	uint16_t dat=0;
	
#ifdef DECIMAL_MODE
	uint16_t  x1,x2,x3;
	uint16_t  r1,r2,r3;
#endif
	
	count++;
	if(count == 50){ //200Hz - 5us pulse x 4 = 20ms - 50Hz
		count = 0;
		
#ifdef HEXADECIMAL_MODE
		switch(index){
			case 0:
				dat = value&0xf000;
				dat >>= 12;
				break;
			case 1:
				dat = value&0xf00;
				dat >>= 8;
				break;
			case 2:
				dat = value&0xf0;
				dat >>= 4;
				break;
			case 3:
				dat = value&0xf;
				break;
		}
#else
		x1 = value/1000;
		r1 = value - x1*1000;
		x2 = r1/100;
		r2 = r1 - x2*100;
		x3 = r2/10;
		r3 = r2 - x3*10;

		switch(index){
			case 0:
				dat = x1;
				break;
			case 1:
				dat = x2;
				break;
			case 2:
				dat = x3;
				break;
			case 3:
				dat = r3;
				break;
		}

#endif
#ifdef CC_MODE
		PORTB = num[dat];
#else
		PORTB = ~num[dat];
#endif		
		PORTD = seg[index];
		index++;
		if(index==4)
			index = 0;
	}
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
			TIMSK0 &= ~(1<<OCIE0A);
			return;
		}
	}
}
