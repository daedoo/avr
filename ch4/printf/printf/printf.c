/*
 * printf.c
 *
 * Created: 2016-01-17 오후 12:20:13
 * Author : sela
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU  18432000L
#include <util/delay.h>
#include "uart.h"

int main(void)
{
 	uart_init();
 		
 	sei();
	 
	while(1){
		printf("Z");
		_delay_ms(1000);
	}
	
}

