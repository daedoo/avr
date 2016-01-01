/*
 * note_a.c
 *
 * Created: 2015-11-19 오후 12:42:39
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     4000000L
#include <util/delay.h>

#define OUTBIT	(1<<PD5)

int main(void)
{
	DDRD = OUTBIT;
	
	// clear counter on compare match, toggle output OC0B on match
	TCCR0A = (1<<COM0B0)|(1<<WGM01); 
	// prescale clk/64 
	TCCR0B = (1<<CS01)|(1<<CS00);
	
	OCR0A = 62500/440;
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

