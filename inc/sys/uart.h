#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

void uart_init(uint32_t baud);

int uart_putchar(int c);

int uart_getchar(void);

int uart_puts(const char *s);

#endif /* __UART_H__ */
