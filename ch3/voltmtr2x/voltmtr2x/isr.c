#include <avr/io.h>
#include <avr/interrupt.h>
#include "7seg.h"

ISR (TIMER0_COMPA_vect)
{
	static uint8_t count = 0;
	static uint8_t index = 0;
	
	count++;
	if(count == 50){ //200Hz - 5us pulse x 4 = 20ms - 50Hz
		count = 0;

		PORTB = get_seg_data(index);
		PORTD = get_seg(index);	
		index++;
		if(index==4)
			index = 0;
	}
}
