#include <FreeRTOS.h>
#include <task.h>
#include <i2c.h>

#include <max1704x.h>
#include <stm32f10x.h>

#include <device_drivers.h>

void init_wireup() {
    
    #ifdef conf_ENABLE_PERIPH_TERM_USART
    term_Install();
    #endif /* conf_ENABLE_PERIPH_TERM_USART */


    /** INSTALL: /dev/GPS **/
        //USART_InitStruct.USART_BaudRate = 9600;
        //USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        //USART_InitStruct.USART_StopBits = USART_StopBits_1;
        //USART_InitStruct.USART_Parity = USART_Parity_No;
        //USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        //USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        //dev_mknode("/dev/GPS", serial_uart, USARTb, &USART_InitStruct);
    /** END: /dev/GPS**/

    /** INSTALL: FuelGadge **/
        //I2C_InitTypeDef I2C_InitStruct;
        //I2C_InitStruct.I2C_ClockSpeed = 100000;
        //I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
        //I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
        //I2C_InitStruct.I2C_OwnAddress1 = 0x3c;
        //I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
        //I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
        //i2c_Install(0, &I2C_InitStruct);
    /** END: FuelGadge **/
}
