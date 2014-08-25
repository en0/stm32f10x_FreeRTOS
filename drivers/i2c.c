#include <errno.h>
#undef errno
extern int errno;

#include <FreeRTOS.h>
#include <stm32f10x.h>
#include <stm32f10x_i2c.h>
#include <gpio.h>
#include <string.h>
#include <i2c.h>

#include <stdlib.h>

typedef struct {
    /** Identify the I2C to use **/
    I2C_TypeDef* I2Cx;

    /** For GPIO Pin Timer Settings **/
    uint32_t Periph_GPIO;
    void(*PeriphClockCmd_GPIO)(uint32_t, FunctionalState);

    /** For I2C Timer Settings **/
    uint32_t Periph_I2C;
    void(*PeriphClockCmd_I2C)(uint32_t, FunctionalState);

    /** The GPIO Pin Settings **/
    GPIO_TypeDef* GPIO_Port;
    uint16_t SDA_Pin;
    uint16_t SCL_Pin;

    /** Tracking **/
    uint32_t FLAGS;

    /** The currently active configuration **/
    I2C_InitTypeDef *I2C_Conf;
} I2C_Handle_t;


/** 
 ** The stm32f103cbt6 has 3 usarts 
 ** This array holds the definitions to
 ** initialize them.
 **/

I2C_Handle_t _i2cs[] = {
    {
        I2C1,                        // I2Cx
        RCC_APB2Periph_GPIOB,        // Periph_GPIO
        RCC_APB2PeriphClockCmd,      // PeriphClockCmd_GPIO
        RCC_APB1Periph_I2C1,         // Periph_I2C
        RCC_APB1PeriphClockCmd,      // PeriphClockCmd_I2C
        GPIOB,                       // GPIO_Port
        GPIO_Pin_7,                  // SDA_Pin
        GPIO_Pin_6,                  // SCL_Pin
        0x00,                        // Flags
        NULL,                        // I2C_Conf
    }
};


void _i2c_InitHw(uint32_t i2cx) {

    I2C_Handle_t *ident = &_i2cs[i2cx];

    // Enabling Clocks
    ident->PeriphClockCmd_GPIO(ident->Periph_GPIO, ENABLE);
    ident->PeriphClockCmd_I2C(ident->Periph_I2C, ENABLE);

    // Setting up pins
    gpio_config(ident->GPIO_Port, ident->SDA_Pin | ident->SCL_Pin, GPIO_Mode_AF_OD, GPIO_Speed_10MHz);

    // Initialzing bus
    I2C_DeInit(ident->I2Cx);           
    I2C_Cmd(ident->I2Cx, ENABLE);
    I2C_Init(ident->I2Cx, ident->I2C_Conf);

    ident->FLAGS |= I2C_ENABLED;
}


void _i2c_wait(int i2cx) {

    int i;
    I2C_Handle_t *ident = &_i2cs[i2cx];

    // Set GPIO to Push Pull
    gpio_config(ident->GPIO_Port, ident->SCL_Pin, GPIO_Mode_Out_PP, GPIO_Speed_10MHz);

    // Manualy clock the SCL to force out of unknown state.
    for(i = 0; i < 8; i++) {
        GPIO_WriteBit(ident->GPIO_Port, ident->SCL_Pin, 0);
        GPIO_WriteBit(ident->GPIO_Port, ident->SCL_Pin, 1);
    }

    _i2c_InitHw(i2cx);
}

void i2c_BufferedRead(int i2cx, uint8_t slave, uint8_t *buffer, uint8_t addr, int count) {

    I2C_Handle_t *ident = &_i2cs[i2cx];

    //while(I2C_GetFlagStatus(ident->I2Cx, I2C_FLAG_BUSY));
    while(I2C_GetFlagStatus(ident->I2Cx, I2C_FLAG_BUSY));

    //Send Start Signal
    I2C_GenerateSTART(ident->I2Cx, ENABLE);

    //Wait untill Start squence is complete
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    //Send Slave Identity
    I2C_Send7bitAddress(ident->I2Cx, slave, I2C_Direction_Transmitter);

    //Wait untill transmitter mode is active
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    //Set PE Bit
    I2C_Cmd(ident->I2Cx, ENABLE);
    
    //Send Register Address to read from
    I2C_SendData(ident->I2Cx, addr);

    //Wait for transmission
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    
    //Re-Start signal for READ action
    I2C_GenerateSTART(ident->I2Cx, ENABLE);

    //Wait untill Start squence is complete
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    //Send address for read
    I2C_Send7bitAddress(ident->I2Cx, slave, I2C_Direction_Receiver);

    //Wait untill reciever status is active
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    //Read n Bytes
    while(count) {

        //If last byte, end Read Session.
        if(count == 1) {

            //Disable Acknowledge
            I2C_AcknowledgeConfig(ident->I2Cx, DISABLE);

            //Send Stop Signal
            I2C_GenerateSTOP(ident->I2Cx, ENABLE);
        }

        //Wait for RECEIVE to become ready
        if(I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)) {

            //Read byte and increment pointer.
            *buffer++ = I2C_ReceiveData(ident->I2Cx);
    
            //Decrement the number of bytes left to read
            count--;
        }
    }

    I2C_AcknowledgeConfig(ident->I2Cx, ENABLE);

    // Reset if needed
    if(I2C_GetFlagStatus(ident->I2Cx, I2C_FLAG_BUSY))
        _i2c_wait(i2cx);
}

void i2c_WriteByte(int i2cx, uint8_t slave, uint8_t buffer, uint8_t addr) {

    I2C_Handle_t *ident = &_i2cs[i2cx];

    //Wait for bus to become available.
    while(I2C_GetFlagStatus(ident->I2Cx, I2C_FLAG_BUSY));

    //Send START Signal
    I2C_GenerateSTART(ident->I2Cx, ENABLE);

    //wait for start signal to complete
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

    //Send Address for write
    I2C_Send7bitAddress(ident->I2Cx, slave, I2C_Direction_Transmitter);

    //Wait for transitter to become ready
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    //Send Register Address to write
    I2C_SendData(ident->I2Cx, addr);

    //Wait for Transmitter to become ready
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    //Send byte to write
    I2C_SendData(ident->I2Cx, buffer);

    //Wait for Transmitter to become ready
    while(!I2C_CheckEvent(ident->I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    //Send stop signal
    I2C_GenerateSTOP(ident->I2Cx, ENABLE);
}

int i2c_Install(uint32_t i2c_no, I2C_InitTypeDef *conf) {

    if(i2c_no >= I2Cx_COUNT) { errno = ENXIO; return -1; }
    I2C_Handle_t *ident = &_i2cs[i2c_no];

    // Make sure it is not already configured
    if((ident->FLAGS & I2C_ENABLED) == I2C_ENABLED) {
        errno = EBUSY; return -1;
    }

    // Store a copy of the config incase we need to reset
    ident->I2C_Conf = malloc(sizeof(I2C_InitTypeDef));
    memcpy(ident->I2C_Conf, conf, sizeof(I2C_InitTypeDef));

    // Initialize hardware
    _i2c_InitHw(i2c_no);

    return i2c_no;
}
