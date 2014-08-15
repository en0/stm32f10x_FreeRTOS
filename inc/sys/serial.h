#ifndef __SERIAL_H
#define __SERIAL_H 1

#include <uart.h>

int serial_uart_open(device_t *dev);
int serial_uart_close(struct _device_t*);
int serial_uart_read(struct _device_t*, uint16_t *v);
int serial_uart_write(struct _device_t*, uint16_t v);
int serial_uart_init(device_t *dev, USART_InitTypeDef* conf);

void serial_i2c_init(device_t *dev, void* conf);
void serial_spi_init(device_t *dev, void* conf);

#endif /* __SERIAL_H */

