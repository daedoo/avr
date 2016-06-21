/*
 * potentio.c
 *
 * Created: 2016-05-08 오후 5:38:36
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     18432000L
#include <util/delay.h>

#define DD_SPI DDRB
#define DD_MOSI PB3
#define DD_SCK	PB5
#define DD_SS	PB2

#define PORT_SPI PORTB

void write_spi(uint8_t byt)
{
	SPDR = byt;
	while(!(SPSR & (1<<SPIF)))
		;
}

int main(void)
{
	uint8_t cnt = 0;
	// set output for spi control port, MOSI-PB3, SCK-PB5, /SS-PB2
	DD_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS);
	// set /SS pin high
	PORT_SPI = (1<<DD_SS);
	
	// SPI ENable, DORD MSB first, MSTR master mode,
	// CPOL sck low when idle, CPHA sample on rising edge, sck = fosc/4
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);

    while (1) 
    {
		while(cnt<255){
			PORT_SPI &= ~(1<<DD_SS);
			write_spi(0x11);
			write_spi(cnt++);
			PORT_SPI |= (1<<DD_SS);
			_delay_ms(10);
		};
		while(cnt>0){
			PORT_SPI &= ~(1<<DD_SS);
			write_spi(0x11);
			write_spi(cnt--);
			PORT_SPI |= (1<<DD_SS);
			_delay_ms(10);
		};

    }
}

