#include <errno.h>
#undef errno
extern int errno;

#include <uart.h>
#include <device.h>

int serial_uart_open(device_t *dev) {
    if(usart_Open(dev->minor) == dev->minor) return 1;
    return -1;
}

int serial_uart_close(struct _device_t* dev) {
    return usart_Close(dev->minor);
}

int serial_uart_read(struct _device_t* dev, uint16_t *v) {
    return usart_ReadByte(dev->minor, v);
}

int serial_uart_write(struct _device_t* dev, uint16_t v) {
    return usart_WriteByte(dev->minor, v);
}


int serial_uart_init(device_t *dev, USART_InitTypeDef* conf) { 
    if((conf->USART_Mode & USART_Mode_Rx) == USART_Mode_Rx)
        dev->mode |= dev_read;

    if((conf->USART_Mode & USART_Mode_Tx) == USART_Mode_Tx)
        dev->mode |= dev_write;

    if(install_USART(dev->minor, conf, 32) <= 0) return -1;

    dev->open = serial_uart_open;
    dev->close = serial_uart_close;
    dev->write = serial_uart_write;
    dev->read = serial_uart_read;

    return 1;
}

void serial_i2c_init(device_t *dev, void* conf) {}
void serial_spi_init(device_t *dev, void* conf) {}

