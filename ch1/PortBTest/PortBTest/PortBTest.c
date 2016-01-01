/*
 * PortBTest.c
 *
 * Created: 2015-10-14 오후 5:47:04
 * Author : sela
 */ 

#include <avr/io.h>


int main(void)
{
    DDRB = 0xff;
	PORTB = 0xa5;
	
    while (1) 
    {
    }
}

