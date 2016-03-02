#include <avr/io.h>
#include "adc.h"


void adc_init(void)
{
	// AVcc with ext. cap at AREF, use ADC0 input
	//ADMUX = (1<<REFS0)|(1<<ADLAR);
	ADMUX = (1<<REFS0);
	// ADC Enable, ADC start Conversion, prescaler 18.432M/64 = 288kHz
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1);
	// Disable unused ADC input
	DIDR0 = (1<<ADC5D)|(1<<ADC4D)|(1<<ADC3D)|(1<<ADC2D);
}

uint16_t adc_read(uint8_t channel)
{
	uint16_t	adc_value;
	
	if(channel > 6)
		return 0;

	//ADMUX = (1<<REFS0)|(1<<ADLAR)|channel;
	ADMUX = (1<<REFS0)|channel;
	ADCSRA = (1<<ADEN)|(1<<ADSC)|(1<<ADPS2)|(1<<ADPS1);
	// wait for conversion end
	while (!(ADCSRA & (1<<ADIF)));
	// clear ADC interrupt flag
	ADCSRA |= (1<<ADIF);
	// read left justified 8bit
	adc_value = ADCL;
	adc_value += ADCH*256;

	return adc_value;
}