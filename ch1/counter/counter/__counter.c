/*
 * counter.c
 *
 * Created: 2015-10-31 오후 9:51:50
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     1000000L
#include <util/delay.h>

#define KEYBIT		(1<<PD2)


int main(void)
{
	uint8_t keyin;
	uint8_t counter = 0;
	
	DDRB = 0xff;
	DDRD = 0;
	PORTD = KEYBIT; // add built-in pull-up
	
	while (1)
	{
		keyin = PIND & KEYBIT;
		
		if(keyin == 0){ // if key pressed
			counter += 1;
			PORTB = counter;
		}
		_delay_ms(20);
	}
}

