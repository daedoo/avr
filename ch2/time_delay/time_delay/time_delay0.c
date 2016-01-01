/*
 * time_delay.c
 *
 * Created: 2015-11-28 오후 3:00:03
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t g_tcount = 0;

void time_delay(uint32_t us);

int main(void)
{
    uint8_t value = 0;
	
	DDRB = 0xff;
		
	// CTC mode 
	TCCR0A = (1<<WGM01);
	// clk source, 4MHz/8
	TCCR0B = (1<<CS01);
	
	// output compare value for 100us 
	OCR0A = 50;
	
	// timer/counter0 output compare match A interrupt enable
	TIMSK = (1<<OCIE0A);
	
	sei();
		
    /* Replace with your application code */
    while (1) 
    {
		PORTB = value;
		// delay 0.5sec
		time_delay(5000);
		
	    value += 1;
    }
}

ISR (TIMER0_COMPA_vect)
{
	g_tcount++;	
}

void time_delay(uint32_t delay)
{
	uint32_t tcount;

	tcount = g_tcount;
	tcount += delay;

	while(1)
	{
		if(g_tcount >= tcount)
			return;
	}
} 