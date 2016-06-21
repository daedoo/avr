/*
 * joypant.c
 *
 * Created: 2016-06-04 오후 3:53:44
 * Author : sela
 */ 

#include <stdio.h>
#define F_CPU     18432000L
#include <util/delay.h>

#include <avr/io.h>
#include "adc.h"
#include "uart.h"

int main(void)
{
	uint16_t adc0_val, adc1_val;
	uint16_t pwm0, pwm1;

	uart_init();
	adc_init();

	DDRB |= (1<<PB1)|(1<<PB2);
    // Fast PWM mode 14, TOP set by ICR1
    TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(1<< WGM11);
    // prescaler 18.432MHz/64 = 288khz
    TCCR1B = (1<<WGM13)|(1<< WGM12)|(1<< CS11)|(1<< CS10);
    // TOP defined by ICR1, 288kHz/5760 = 50Hz (20ms)
    ICR1 = 5760;


    while (1) 
    {
		 adc0_val = adc_read(0); // pan motor
		 adc1_val = adc_read(1); // tilt motor

		 pwm0 = (int)(0.563*(float)adc0_val) + 130;
		 //pwm1 = (int)(0.268*(float)adc1_val) + 432;
		 if(adc1_val >= 512)
			pwm1 = (int)(0.563*(float)adc1_val) + 130;
		else
			pwm1 = 418;

		 printf("adc0: %d, adc1: %d\n", adc0_val, adc1_val);
		 printf("pwm0: %d, pwm1: %d\n", pwm0, pwm1);

		 OCR1A = pwm0;
		 OCR1B = pwm1;

		 _delay_ms(50);
		
    }
}

