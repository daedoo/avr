/*
 * keyin.c
 *
 * Created: 2015-10-31 오후 4:27:10
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     1000000L
#include <util/delay.h>

#define LEDBIT      (1<<PB7)
#define KEYBIT		(1<<PD2)

int main(void)
{
	uint8_t keyin;
	
	DDRB = LEDBIT;
	DDRD = 0;
	PORTD = KEYBIT; // add built-in pull-up 
	
    while (1) 
    {
		keyin = PIND & KEYBIT;
		
		if(keyin == 0) // if key pressed
			PORTB |= LEDBIT;
		else
			PORTB &= ~LEDBIT;
		_delay_ms(20);						
    }
}

