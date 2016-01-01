/*
 * keyint.c
 *
 * Created: 2015-11-09 오후 3:00:39
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     1000000L
#include <util/delay.h>


int main(void)
{
    uint8_t value;
    uint8_t mode = 0;

	DDRB = 0xff;

    if(mode==0)
        value = 1;
    else
        value = 0x80;

	while (1)
	{
        PORTB = value;
		_delay_ms(200);

        if(mode==0){    
            value <<= 1;
            if(value==0)
                value = 1;
        }else{
            value >>= 1;
            if(value==0)
                value = 0x80;
        }  
	}	
}




