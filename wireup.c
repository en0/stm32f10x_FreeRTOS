#include <uart.h>
#include <device.h>



void init_wireup() {

    // Create a device map
    init_devicemap(10);

    USART_InitTypeDef conf;

    /** INSTALL: /dev/ttyUART3 **/
        conf.USART_BaudRate = 115200;
        conf.USART_WordLength = USART_WordLength_8b;
        conf.USART_StopBits = USART_StopBits_1;
        conf.USART_Parity = USART_Parity_No;
        conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        conf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        dev_mknode("/dev/ttyUART3", serial_uart, USARTc, &conf);
    /** END: /dev/ttyUART3 **/


    /** INSTALL: /dev/GPS **/
        conf.USART_BaudRate = 9600;
        //conf.USART_WordLength = USART_WordLength_8b;
        //conf.USART_StopBits = USART_StopBits_1;
        //conf.USART_Parity = USART_Parity_No;
        //conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        //conf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        dev_mknode("/dev/GPS", serial_uart, USARTb, &conf);
    /** END: /dev/GPS**/
}
