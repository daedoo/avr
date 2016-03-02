#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint32_t g_tcount = 0;

		
ISR (TIMER0_COMPA_vect)
{
	g_tcount++;	
}

uint32_t get_ms(void)
{
	return g_tcount/10;
}
