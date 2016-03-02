/*
 * uartcmd.c
 *
 * Created: 2016-01-18 오후 10:09:08
 * Author : sela
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU     18432000L
#include <util/delay.h>
#include "uart.h"

extern void cmd_loop(void);

int main(void)
{
	char c;
 	uart_init();
	 	
	printf("ATmega328P monitor\n");
	cmd_loop();
}

