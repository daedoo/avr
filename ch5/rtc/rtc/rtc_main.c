/*
 * rtc.c
 *
 * Created: 2016-04-09 오후 4:09:47
 * Author : sela
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"
#include "i2c.h"
#include "cmd.h"

int main(void)
{
  	uart_init();
	i2c_init(100);
  	
  	printf("ATmega328P monitor\n");
  	cmd_loop();
}
