/*
 * svrm_pwm.c
 *
 * Created: 2015-12-03 오후 5:33:47
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU     4000000L
#include <util/delay.h>

#define SRVMBIT (1<<PB3) // OCR1A output bit


int main(void)
{
	uint16_t count = 750;
	int i;
	
	DDRB = SRVMBIT;
	
	// Fast PWM mode, Clear OC0A,B on Compare Match, set OC0A,B at 0xff
	// TOP set by ICR1 
	TCCR1A = (1<<COM1A1)|(1<<WGM11);
	// prescaler 4MHz/8 = 500khz
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);
	// TOP defined by ICR1, 500kHz/10000 = 50Hz (20ms)
	ICR1 = 10000;
	
    while (1) 
    {
		count = 750;
		
		for(count=750;count<750+85*7;count+=85){
			OCR1A = count;
			_delay_ms(1000);			
		}
		for(count=750;count>750-85*7;count-=85){
			OCR1A = count;
			_delay_ms(1000);
		}		
    }
}

