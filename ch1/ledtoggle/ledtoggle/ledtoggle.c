/*
 * ledtoggle.c
 *
 * Created: 2015-10-31 오후 12:01:17
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     1000000L
#include <util/delay.h>

#define LEDBIT          (1<<PB7)

int main(void)
{
	DDRB = LEDBIT ;
	
	while(1)
	{
		PORTB |= LEDBIT ;
		_delay_ms(1000);
		PORTB &= ~LEDBIT ;
		_delay_ms(1000);
	}
}

