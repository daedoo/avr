/*
 * led_pwm.c
 *
 * Created: 2015-11-30 오후 5:32:56
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU     4000000L
#include <util/delay.h>

#define LED0BIT (1<<PB2)
#define LED1BIT (1<<PD5)

#define INC	0
#define DEC 1

int main(void)
{
	uint8_t led, mode;
	
	DDRB = LED0BIT;
	DDRD = LED1BIT;
	
	// Fast PWM mode, Clear OC0A,B on Compare Match, set OC0A,B at 0xff
	TCCR0A = (1<<COM0A1)|(1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	// clk source, pwm freq = 4MHz/(8*256) = 1.95khz
	TCCR0B = (1<<CS01);	
	
	led = 0;
	mode = INC;
		
    /* Replace with your application code */
    while (1) 
    {
		OCR0A = led;
		OCR0B = 180-led;
		_delay_ms(10);
		if(mode==INC){
			if(led==180){
				mode = DEC;	
				led--;
			}
			else
				led++;
		} else { //mode==DEC
			if(led==0){
				mode = INC;
				led++;
			}
			else
				led--;			
		}
    }
}

