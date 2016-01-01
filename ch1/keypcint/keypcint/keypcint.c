/*
 * keypcint.c
 *
 * Created: 2015-11-09 오후 3:18:08
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU     1000000L
#include <util/delay.h>

#define KEYBIT (1<<PB7)

static uint8_t mode = 0;

int main(void)
{
    uint8_t value = 1;
	DDRB = 0x7f;
	PORTB = KEYBIT;
	
	PCMSK = (1<<PCINT7);    // enable pcint7
	GIMSK = (1<<PCIE);      // enable pcint
	
	sei();
	
	while (1)
	{
        PORTB = value|KEYBIT;
        _delay_ms(100);
		
        if(mode==0){
	        value <<= 1;
	        if(value==0x80)
				value = 1;
	    }else{
	        value >>= 1;
	        if(value==0)
				value = 0x40;
        } 
	}
}

ISR (PCINT_vect)
{	
    _delay_ms(20);	
	if((PINB&KEYBIT)==0)
    {
        if(mode == 0)
            mode = 1;
        else
            mode = 0;
    }
}



