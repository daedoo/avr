/*
 * i2c_slave.cpp
 *
 * Created: 2016-04-18 오후 3:13:12
 * Author : sela
 */ 
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "uart.h"
#include "i2c.h"


uint8_t avr_buffer[16];

int main(void)
{
	uart_init();
	
	printf("Start avr i2c slave...\n");
	sei();
	
	i2c_slave_init(0x10, avr_buffer, 16);	
	while(1);
}

