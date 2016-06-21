#ifndef _UART_H_
#define _UART_H_

#ifdef __cplusplus
extern "C" {
#endif    

#define RX_BUFSIZE 80

void uart_init(void);
int	uart_putchar(char c);
int	uart_getchar(void);
uint8_t uart_kbhit(void);


#ifdef __cplusplus
}	
#endif

#endif

