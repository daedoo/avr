/*
 * spi_adc.c
 *
 * Created: 2016-05-15 오전 11:31:02
 * Author : sela
 */ 

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "uart.h"

#define DD_SPI DDRB
#define DD_MOSI PB3
#define DD_SCK	PB5
#define DD_SS0	PB2
#define DD_SS1	PB0

#define PORT_SPI PORTB

void write_spi(uint8_t byt)
{
	SPDR = byt;
	while(!(SPSR & (1<<SPIF)))
	;
}

uint16_t read_spi_adc(uint8_t ch)
{
	uint8_t byt[3];
	uint8_t val1,val2;
	
	byt[0] = 1;
	if(ch>0) ch = 0x2;
	byt[1] = 0b101+ch;
	byt[1] <<=5;
	byt[2] = 0;
		
	PORT_SPI &= ~(1<<DD_SS1);	
	write_spi(byt[0]);		
	write_spi(byt[1]);
	val1 = SPDR&0xf;
	write_spi(byt[2]);
	val2 = SPDR;
	PORT_SPI |= (1<<DD_SS1);	
	
	return val1*256+val2;
}

int main(void)
{
	uint16_t adc;
	
	// set output for spi control port, MOSI-PB3, SCK-PB5, /SS-PB0
	DD_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS0)|(1<<DD_SS1);
	// set /SS pin high
	PORT_SPI = (1<<DD_SS0)|(1<<DD_SS1);
	
	// SPI Enable, DORD MSB first, MSTR master mode,
	// CPOL sck low when idle, CPHA sample on rising edge, sck = fosc/4
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	
	uart_init();

	while (1)
	{
		adc = read_spi_adc(0);
		printf("%d\n", adc);
		_delay_ms(1000);
	}
}

