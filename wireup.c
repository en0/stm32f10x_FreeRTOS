#include <uart.h>
#include <device.h>



void init_wireup() {

    // Create a device map
    init_devicemap(10);


    /** INSTALL: /dev/ttyUSART2 **/
        USART_InitTypeDef conf;
        conf.USART_BaudRate = 115200;
        conf.USART_WordLength = USART_WordLength_8b;
        conf.USART_StopBits = USART_StopBits_1;
        conf.USART_Parity = USART_Parity_No;
        conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        conf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        dev_mknode("/dev/ttyUART2", serial_uart, USARTc, &conf);
    /** END: /dev/ttyUSART2 **/
}
