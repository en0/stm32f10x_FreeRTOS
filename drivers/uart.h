#ifndef __UART_H__
#define __UART_H__

#include <stdint.h>

int uart_putchar(int c);

void uart_init(uint32_t baud);


#endif /* __UART_H__ */
