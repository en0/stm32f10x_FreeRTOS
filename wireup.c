#include <uart.h>
#include <device.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>

#include <stm32f10x.h>
//#include <stm32f10x_usart.h>
//#include <stm32f10x_gpio.h>

extern FILE* _shell_cmd;

void dbg_i2c() {

    GPIO_InitTypeDef GPIO_InitStruct;
    I2C_InitTypeDef I2C_InitStruct;
    
    // Enable Clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

    // Setup pins
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStruct);


    /****************************************
     * 9 Degrees Of Freedome                *
     ****************************************
     * I2C Address 0x1E is the HMC5883L     *
     * I2C Address 0×53 is the ADXL345      *
     * I2C Address 0×69 is the L3G4200D     *
     * I2C Address 0×77 is the BMP085       *
     ****************************************/

    /* I2C Peripheral Enable */          
    I2C_InitStruct.I2C_ClockSpeed = 100000;
    I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStruct.I2C_OwnAddress1 = 0x3c;
    I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    // Enable
    I2C_DeInit(I2C1);           
    I2C_Cmd(I2C1, ENABLE);
    I2C_Init(I2C1, &I2C_InitStruct);
}

void i2c_wait(I2C_TypeDef* I2Cn) {
    int i;

    // Set GPIO to Push Pull
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // Manualy clock the SCL to force out of unknown state.
    for(i = 0; i < 8; i++) {
        GPIO_WriteBit(GPIOB, GPIO_Pin_6, 0);
        GPIO_WriteBit(GPIOB, GPIO_Pin_6, 1);
    }

    // Re-init i2c bus
    dbg_i2c();
}

void i2c_BufferedRead(I2C_TypeDef* I2Cn, uint8_t slave, uint8_t *buffer, uint8_t addr, int count) {
    while(I2C_GetFlagStatus(I2Cn, I2C_FLAG_BUSY));

    //Send Start Signal
    I2C_GenerateSTART(I2Cn, ENABLE);

    //Wait untill Start squence is complete
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_MODE_SELECT));

    //Send Slave Identity
    I2C_Send7bitAddress(I2Cn, slave, I2C_Direction_Transmitter);

    //Wait untill transmitter mode is active
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    //Set PE Bit
    I2C_Cmd(I2Cn, ENABLE);
    
    //Send Register Address to read from
    I2C_SendData(I2Cn, addr);

    //Wait for transmission
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    //Re-Start signal for READ action
    I2C_GenerateSTART(I2Cn, ENABLE);

    //Wait untill Start squence is complete
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_MODE_SELECT));

    //Send address for read
    I2C_Send7bitAddress(I2Cn, slave, I2C_Direction_Receiver);

    //Wait untill reciever status is active
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    //Read n Bytes
    while(count) {

        //If last byte, end Read Session.
        if(count == 1) {

            //Disable Acknowledge
            I2C_AcknowledgeConfig(I2Cn, DISABLE);

            //Send Stop Signal
            I2C_GenerateSTOP(I2Cn, ENABLE);
        }

        //Wait for RECEIVE to become ready
        if(I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_BYTE_RECEIVED)) {

            //Read byte and increment pointer.
            *buffer++ = I2C_ReceiveData(I2Cn);
    
            //Decrement the number of bytes left to read
            count--;
        }
    }

    I2C_AcknowledgeConfig(I2Cn, ENABLE);
    i2c_wait(I2Cn);
}

void i2c_WriteByte(I2C_TypeDef* I2Cn, uint8_t slave, uint8_t buffer, uint8_t addr) {
    //Wait for bus to become available.
    while(I2C_GetFlagStatus(I2Cn, I2C_FLAG_BUSY));

    //Send START Signal
    I2C_GenerateSTART(I2Cn, ENABLE);

    //wait for start signal to complete
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_MODE_SELECT));

    //Send Address for write
    I2C_Send7bitAddress(I2Cn, slave, I2C_Direction_Transmitter);

    //Wait for transitter to become ready
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    //Send Register Address to write
    I2C_SendData(I2Cn, addr);

    //Wait for Transmitter to become ready
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    //Send byte to write
    I2C_SendData(I2Cn, buffer);

    //Wait for Transmitter to become ready
    while(!I2C_CheckEvent(I2Cn, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    //Send stop signal
    I2C_GenerateSTOP(I2Cn, ENABLE);
}



void init_wireup() {

    dbg_i2c();

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
