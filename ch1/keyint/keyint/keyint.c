/*
 * keyint.c
 *
 * Created: 2015-11-09 오후 3:00:39
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU     1000000L
#include <util/delay.h>

#define KEYBIT	(1<<PD2)

static uint8_t mode = 0;

int main(void)
{
    uint8_t value;

	DDRB = 0xff;
	DDRD = 0;
	PORTD = KEYBIT; // add built-in pull-up	
	
	MCUCR = (1<<ISC01); // int0, falling-edge interrupt 
	GIMSK = (1<<INT0); // int0 enabled

    if(mode==0)
        value = 1;
    else
        value = 0x80;

	sei();
	
	while (1)
	{
        PORTB = value;
		_delay_ms(100);

        if(mode==0){    
            value <<= 1;
            if(value==0)
                value = 1;
        }else{
            value >>= 1;
            if(value==0)
                value = 0x80; 
		}
	}	
}

ISR (INT0_vect)
{
	_delay_ms(20);
	
	if((PIND&KEYBIT)==0){
		if(mode==0)
			mode = 1;
		else
			mode = 0;
	}
}



