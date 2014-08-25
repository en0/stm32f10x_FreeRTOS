#ifndef __TRACKERCONFIG_H
#define __TRACKERCONFIG_H 1

/** The max length of a path name including the null terminator **/
#define conf_MAX_PATH_LENGTH 16

/** The max number of open files supported by the system **/
#define conf_MAX_OPEN_FILE_IDS 32

/** REGION: GPS Wireup **/

// Enable GPS over USART
#define conf_ENABLE_PERIPH_GPS_USART 1
#define conf_DEVLIST_PATH_GPS_USART "/dev/ttyUSART2"

// UART Port
#define conf_GPS_USART USART2
#define conf_GPS_USART_IRQn USART2_IRQn

// UART Clock
#define conf_GPS_USART_RCC_Periph_USARTx RCC_APB1Periph_USART2
#define conf_GPS_USART_RCC_PeriphClockCmd RCC_APB1PeriphClockCmd

// GPIO Pins
#define conf_GPS_GPIO GPIOA
#define conf_GPS_GPIO_RX GPIO_Pin_3
#define conf_GPS_GPIO_TX GPIO_Pin_2

// GPIO Clock
#define conf_GPS_GPIO_RCC_Periph_GPIOx (RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO)
#define conf_GPS_GPIO_RCC_PeriphClockCmd_GPIOx RCC_APB2PeriphClockCmd

// USART Init Details
#define conf_GPS_USART_BaudRate 9600
#define conf_GPS_USART_WordLength USART_WordLength_8b
#define conf_GPS_USART_StopBits USART_StopBits_1
#define conf_GPS_USART_Parity USART_Parity_No
#define conf_GPS_USART_HardwareFlowControl USART_HardwareFlowControl_None
#define conf_GPS_USART_Mode (USART_Mode_Rx | USART_Mode_Tx)

/** END REGION: GPS Wireup **/


/** REGION: Terminal Wireup **/

// Enable Termainal IO over USART
#define conf_ENABLE_PERIPH_TERM_USART 1
#define conf_DEVLIST_PATH_TERM_USART "/dev/tty0"

// Uart Port
#define conf_TERM_USART USART3
#define conf_TERM_USART_IRQn USART3_IRQn

// UART Clock
#define conf_TERM_USART_RCC_Periph_USARTx RCC_APB1Periph_USART3
#define conf_TERM_USART_RCC_PeriphClockCmd RCC_APB1PeriphClockCmd

// GPIO Pins
#define conf_TERM_GPIO GPIOB
#define conf_TERM_GPIO_RX GPIO_Pin_11
#define conf_TERM_GPIO_TX GPIO_Pin_10

// GPIO Clock
#define conf_TERM_GPIO_RCC_Periph_GPIOx (RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO)
#define conf_TERM_GPIO_RCC_PeriphClockCmd RCC_APB2PeriphClockCmd

// USART Init Details
#define conf_TERM_USART_BaudRate 115200
#define conf_TERM_USART_WordLength USART_WordLength_8b
#define conf_TERM_USART_StopBits USART_StopBits_1
#define conf_TERM_USART_Parity USART_Parity_No
#define conf_TERM_USART_HardwareFlowControl USART_HardwareFlowControl_None
#define conf_TERM_USART_Mode (USART_Mode_Rx | USART_Mode_Tx)

/** END REGION: TERM Wireup **/


/** REGION: UNUSED USART **/

// UART Port
#define conf_XXXX_USART USART1
#define conf_XXXX_USART_IRQn USART1_IRQn

// UART Clock
#define conf_XXXX_USART_RCC_Periph_USARTx RCC_APB2Periph_USART1
#define conf_XXXX_USART_RCC_PeriphClockCmd RCC_APB2PeriphClockCmd

// GPIO Pins
#define conf_XXXX_GPIO GPIOA
#define conf_XXXX_GPIO_RX GPIO_Pin_10
#define conf_XXXX_GPIO_TX GPIO_Pin_9

// GPIO Clock
#define conf_XXXX_GPIO_RCC_Periph_GPIOx RCC_APB2Periph_GPIOA
#define conf_XXXX_GPIO_RCC_PeriphClockCmd RCC_APB2PeriphClockCmd

/** END REGION: UNUSED USART **/


/** REGION: I2C Bus 1 Wireup **/

// Enable The I2C as Bus 1
#define conf_ENABLE_I2C_BUS1 1

// i2c Port
#define conf_BUS1_I2C I2C1

// i2c Clock
#define conf_BUS1_I2C_RCC_Periph_I2Cx RCC_APB1Periph_I2C1
#define conf_BUS1_I2C_RCC_PeriphClockCmd RCC_APB1PeriphClockCmd

// GPIO Pins
#define conf_BUS1_I2C_GPIO GPIOB
#define conf_BUS1_I2C_SDA GPIO_Pin_7
#define conf_BUS1_I2C_SCL GPIO_Pin_6

// GPIO Clock
#define conf_BUS1_GPIO_RCC_Periph_GPIOx RCC_APB2Periph_GPIOB
#define conf_BUS1_GPIO_RCC_PeriphClockCmd RCC_APB2PeriphClockCmd

// I2C Init Details
#define conf_BUS1_I2C_ClockSpeed 100000
#define conf_BUS1_I2C_Mode I2C_Mode_I2C
#define conf_BUS1_I2C_DutyCycle I2C_DutyCycle_2
#define conf_BUS1_I2C_OwnAddress1 0x3c
#define conf_BUS1_I2C_Ack I2C_Ack_Enable
#define conf_BUS1_I2C_AcknowledgedAddress I2C_AcknowledgedAddress_7bit

/** ENDREGION: I2C Bus 1 Wireup **/


/** REGION: I2C Bus 1 Peripherals **/

// Fuel Gauge
#define conf_ENABLE_PERIPH_FUELGAUGE_I2C 1
#define conf_FUELGAUGE_BUS conf_BUS1_I2C

/** END REGION: I2C Bus 1 Peripherals **/


#endif /* __TRACKERCONFIG_H */


