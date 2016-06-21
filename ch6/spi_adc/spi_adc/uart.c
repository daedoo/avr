
#include <stdio.h>
#include <avr/io.h>
#include "uart.h"

#define UART_BAUD  115200
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar_stream, uart_getchar_stream, _FDEV_SETUP_RW);

void uart_init(void)
{
    // set baudrate
	UBRR0L = (F_CPU/(16UL*UART_BAUD)) - 1;
    // enable Tx, Rx 
	UCSR0B = (1<<TXEN0) | (1<<RXEN0);
    // 8N1
	UCSR0C = 3<<UCSZ00;

	// stdio set to uart 
	stdout = stdin = stderr = &uart_str;    
}

int uart_putchar(char c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	
	return 0;
}

int uart_getchar(void)
{
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
}

uint8_t uart_kbhit(void)
{
    uint8_t hit = 0;

    if(UCSR0A & (1<<RXC0)) hit = 1;
    return hit;
}

int uart_putchar_stream(char c, FILE *stream)
{
	if (c == '\n')
		uart_putchar_stream('\r', stream);
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
	
	return 0;
}

int uart_getchar_stream(FILE *stream)
{
	uint8_t c, i;
	char *cp, *tp;
	static char rxb[RX_BUFSIZE];
	static char *bp;

    // no data in buffer - wait for data stream from uart
    if(bp == 0)
    {
        cp = rxb;
        while(1)
		{
			loop_until_bit_is_set(UCSR0A, RXC0);
			if (UCSR0A & (1<<FE0))
				return _FDEV_EOF;
			if (UCSR0A & (1<<DOR0))
				return _FDEV_ERR;
            // get data
			c = UDR0;
			
			// backspace
			if(c=='\b')
			{
				if(cp>rxb){
					cp--;
					// VT100 - move cursor left
					uart_putchar_stream(0x1b, stream);
					uart_putchar_stream('[', stream);				
					uart_putchar_stream('D', stream);
					// VT100 - erase cursor to end of the line
					uart_putchar_stream(0x1b, stream);
					uart_putchar_stream('[', stream);
					uart_putchar_stream('K', stream);					
				}
			}
			
			// check escape sequence - handle VT100 cursor up
			if(c==0x1b){
				loop_until_bit_is_set(UCSR0A, RXC0);
				c = UDR0;
				
				if(c=='['){
					loop_until_bit_is_set(UCSR0A, RXC0);
					c = UDR0;			
				}
				if(c=='A'){
					tp = cp;
					for(i=0;i<(tp-rxb);i++){
						// VT100 - move cursor left
						cp--;
						uart_putchar_stream(0x1b, stream);
						uart_putchar_stream('[', stream);
						uart_putchar_stream('D', stream);	
						// VT100 - erase cursor to end of the line
						uart_putchar_stream(0x1b, stream);
						uart_putchar_stream('[', stream);
						uart_putchar_stream('K', stream);											
					}
					// setup buffer
					*cp++ = '[';
					*cp++ = 'A';
					*cp = '\n';
					bp = rxb;
					break;
				}
			}
            
            // convert 'return' to 'new line'
			if (c == '\r')
				c = '\n';

            // clear buffer and return on 'new line'
			if (c == '\n')
			{
				*cp = c;
				uart_putchar_stream(c, stream);
				bp = rxb;
				break;
			}

            // store printable chars 0x20 to 0x7e
			if((c >= (uint8_t)' ') && (c <= (uint8_t)'~'))
			{
                if(cp < (rxb + RX_BUFSIZE - 1))
				{
					*cp++ = c;
					uart_putchar_stream(c, stream);
				}
			}
        }
    }
    // return a char from buffer
	c = *bp++;
	if (c == '\n')
		bp = 0;

	return c;
}

