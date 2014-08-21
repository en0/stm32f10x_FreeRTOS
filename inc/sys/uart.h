#ifndef __UART_H__
#define __UART_H__

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stm32f10x_usart.h>

#define USARTx_COUNT 3

enum USARTx {
    USARTa = 0,
    USARTb = 1,
    USARTc = 2,
};

enum USART_Status_Flags {
    USART_ENABLED = 1,
    USART_RX = 2,
    USART_TX = 4,
    USART_ISOPEN = 8,
};


/** 
 ** Read a byte from the recieve queue
 ** Arguments: 
 **  usart_no - The Universal Sync/Async TX/RX identifyer. enum USARTx.
 **  *v       - Pointer to a 16 bit integer that will hold the read value.
 **
 ** Returns:
 **  If successful, 1 will be returned.
 **  If no data was available, a zero will be returned.
 **  otherwise, -1 will be returned and errno will be set.
 **/

int usart_ReadByte(uint32_t usart_no, uint16_t *v);


/**
 ** Write a byte out the uart TX pin.
 ** Arguments:
 **  usart_no - The Universal Sync/Async TX/RX identifyer. enum USARTx.
 **  v        - The value to write.
 **
 ** Returns:
 **  If successfull, a 1 will be returned.
 **  else a -1 will be returned and errno will be set.
 **/

int usart_WriteByte(uint32_t usart_no, uint16_t v);


/** 
 ** Open a USART for Reading or writing depending on its initial configuration.
 ** The queue is flushed when opening.
 ** Arguments:
 **  usart_no - The Universal Sync/Async TX/RX identifyer. enum USARTx.
 **  flags    - The flags to open the file with (O_RDONLY,O_WRONLY,O_RDWR)
 **
 ** Returns: 
 **  If successfull, the usart_no will be returned.
 **  Else, -1 will be returned and errno will be set.
 **/

int usart_Open(uint32_t usart_no, int flags);


/** 
 ** Close a usart no data will be queued on a closed usart. 
 ** This is the initial state of each uart.
 ** Arguments:
 **  usart_no - The Universal Sync/Async TX/RX identifyer. enum USARTx.
 **
 ** Returns: 
 **  NONE
 **/

int usart_Close(uint32_t usart_no);


/** 
 ** Initialize a uart with the provided config. The usart is initialized in a closed
 ** state. If RX is defined, a recieve queue will be set up and an interrupt handler
 ** to fill the queue.
 ** Arguments:
 **  usart_no - The Universal Sync/Async TX/RX identifyer. enum USARTx.
 **  *conf    - The USART_InitTypeDef configuration of the Art specified.
 **  queueSize- The size of the recieve queue.
 **
 ** Returns: 
 **  If successfull, the usart_no will be returned.
 **  Else, -1 will be returned and errno will be set.
 **/

int install_USART(uint32_t usart_no, USART_InitTypeDef *conf, uint32_t queueSize);

#endif /* __UART_H__ */
