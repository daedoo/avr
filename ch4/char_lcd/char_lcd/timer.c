/*
 * timer.c
 *
 * Created: 2016-01-02 오후 2:12:41
 *  Author: sela
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t g_tcount = 0;

void timer_init(void)
{
	// set timer0, CTC mode
	TCCR0A = (1<<WGM01)|(1<<COM0A0);
	// clk source, freq = 18.432MHz/8 = 230.4kHz
	TCCR0B = (1<<CS01);
	// output compare value for 100us
	OCR0A = 230;  // 10.017kHz
	
	TIMSK0 |= (1<<OCIE0A);
}

uint32_t get_ms(void)
{
	return (uint32_t)((double)g_tcount/9.765);
}

		
ISR (TIMER0_COMPA_vect)
{
	g_tcount++;
}

