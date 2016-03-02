/*
 * adc_swx3.c
 *
 * Created: 2016-01-05 오후 4:43:38
 * Author : sela
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU     8000000L
#include <util/delay.h>

#include "7seg.h"

#define OPEN_H (255*3/4 + 5)  // 60-70
#define OPEN_L (255*3/4 - 5)
#define SW1_H (255*2/3 + 5)   // 80-90
#define SW1_L (255*2/3 - 5)
#define SW2_H (255/2 + 5)   // 122-132
#define SW2_L (255/2 - 5)
#define SW3_H (0 + 5)
#define SW3_L (0)

uint8_t adc_read(uint8_t channel);

int main(void)
{
	uint8_t value;
	//uint16_t disp_value;
	
	DDRB = 0xff;
	DDRD = (1<<PD3)|(1<<PD4)|(1<<PD7)|(1<<PD5);
	
	// CTC mode
	TCCR0A = (1<<WGM01);
	// clk source, 8MHz/8 = 500kHz
	TCCR0B = (1<<CS01);
	// output compare value for 100us
	OCR0A = 100;  // 10kHz
	TIMSK0 |= (1<<OCIE0A);
	
	// AVcc with ext. cap at AREF, use ADC0 input
	ADMUX = (1<<REFS0)|(1<<ADLAR);
	// ADC Enable, ADC start Conversion, prescaler 8M/64 = 125kHz
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1);
	// Disable unused ADC input
	DIDR0 = (1<<ADC5D)|(1<<ADC4D)|(1<<ADC3D)|(1<<ADC2D)|(1<<ADC1D);
	
	sei();
	
	while (1)
	{
		value = adc_read(ADC1D);
		
		if((value<=SW1_H)&&(value>=SW1_L)) //check sw1
		{
			//state = 1;
			set_7seg(1, 0, 0);
			_delay_ms(20);
		}
		else if((value<=SW2_H)&&(value>=SW2_L)) //check sw2
		{
			//state = 2;
			set_7seg(2, 0, 0);
		_delay_ms(20);		}
		else if((value<=SW3_H)&&(value>=SW3_L)) //check sw3
		{
			//state = 3;
			set_7seg(3, 0, 0);
			_delay_ms(20);
		}
		else // maybe open
		{
			set_7seg(0, 0, 0);
			_delay_ms(20);
		}
	}
}

uint8_t adc_read(uint8_t channel)
{
	uint8_t	adch;
	
	if(channel > 7)
	return 0;

	ADMUX = (1<<REFS0)|(1<<ADLAR)|channel;
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1);
	// wait for conversion end
	while (!(ADCSRA & (1<<ADIF)));
	// clear ADC interrupt flag
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADIF)|(1<<ADPS2)|(1<<ADPS1);
	// read left justified 8bit
	adch = ADCH;

	return adch;
}