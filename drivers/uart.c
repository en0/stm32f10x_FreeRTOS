#include <errno.h>
#undef errno
extern int errno;

#include <stm32f10x.h>
#include <stm32f10x_usart.h>
#include <gpio.h>
#include <uart.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>


/**
 ** Initialize a specific UART. Init for RX or TX or RX/TX
 ** Should except arguments for USART Config
 ** should except argument for interrupt return
 ** expose send function
 ** expose recieve function
 **/

typedef struct {
    /** Identify the USART to use **/
    USART_TypeDef* USARTx;

    /** IRQ Handle **/
    uint32_t GlobalIRQn;

    /** For GPIO Pin Timer Settings **/
    uint32_t Periph_GPIO;
    void(*PeriphClockCmd_GPIO)(uint32_t, FunctionalState);

    /** For USART Timer Settings **/
    uint32_t Periph_USART;
    void(*PeriphClockCmd_USART)(uint32_t, FunctionalState);

    /** The GPIO Pin Settings **/
    GPIO_TypeDef* GPIO_Port;
    uint16_t RX_Pin;
    uint16_t TX_Pin;

    /** Tracking **/
    uint32_t FLAGS;
    xQueueHandle* rx;
} USART_Handle_t;


/** 
 ** The stm32f103cbt6 has 3 usarts 
 ** This array holds the definitions to
 ** initialize them.
 **/

USART_Handle_t _usarts[] = {
    {
        USART1,                      // USARTx
        USART1_IRQn,                 // GlobalIRQn
        RCC_APB2Periph_GPIOA,        // Periph_GPIO
        RCC_APB2PeriphClockCmd,      // PeriphClockCmd_GPIO
        RCC_APB2Periph_USART1,       // Periph_USART
        RCC_APB2PeriphClockCmd,      // PeriphClockCmd_USART
        GPIOA,                       // GPIO_Port
        GPIO_Pin_10,                 // RX_Pin
        GPIO_Pin_9,                  // TX_Pin
        0x00,                        // FLAGS
        NULL,                        // rx
    }, {
        USART2,                      // USARTx
        USART2_IRQn,                 // GlobalIRQn
        RCC_APB2Periph_GPIOA,        // Periph_GPIO
        RCC_APB2PeriphClockCmd,      // PeriphClockCmd_GPIO
        RCC_APB1Periph_USART2,       // Periph_USART
        RCC_APB1PeriphClockCmd,      // PeriphClockCmd_USART
        GPIOA,                       // GPIO_Port
        GPIO_Pin_3,                  // RX_Pin
        GPIO_Pin_2,                  // TX_Pin
        0x00,                        // FLAGS
        NULL,                        // rx
    }, {
        USART3,                      // USARTx
        USART3_IRQn,                 // GlobalIRQn
        RCC_APB2Periph_GPIOB,        // Periph_GPIO
        RCC_APB2PeriphClockCmd,      // PeriphClockCmd_GPIO
        RCC_APB1Periph_USART3,       // Periph_USART
        RCC_APB1PeriphClockCmd,      // PeriphClockCmd_USART
        GPIOB,                       // GPIO_Port
        GPIO_Pin_11,                 // RX_Pin
        GPIO_Pin_10,                 // TX_Pin
        0x00,                        // FLAGS
        NULL,                        // rx
    }
};

void _install_IT(USART_TypeDef* USARTx, uint32_t GlobalIRQn) {
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = GlobalIRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
}

void _init_hw(uint32_t usartx, USART_InitTypeDef *conf) {
    USART_Handle_t *ident = &_usarts[usartx];

    ident->PeriphClockCmd_GPIO(ident->Periph_GPIO | RCC_APB2Periph_AFIO, ENABLE);
    ident->PeriphClockCmd_USART(ident->Periph_USART, ENABLE);

    if((conf->USART_Mode & USART_Mode_Rx) == USART_Mode_Rx) {
        ident->FLAGS |= USART_RX;
        gpio_config(ident->GPIO_Port, ident->RX_Pin, GPIO_Mode_IN_FLOATING, 0);
        _install_IT(ident->USARTx, ident->GlobalIRQn);
    }

    if((conf->USART_Mode & USART_Mode_Tx) == USART_Mode_Tx) {
        ident->FLAGS |= USART_TX;
        gpio_config(ident->GPIO_Port, ident->TX_Pin, GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
    }


    USART_Init(ident->USARTx, conf);
    USART_Cmd(ident->USARTx, ENABLE);
    ident->FLAGS |= USART_ENABLED;
}


/**
 ** Interrupt handers for RX data on UARTx
 **/

void _irqhander(uint32_t usart_no, uint16_t c) {
    USART_Handle_t *ident = &_usarts[usart_no];

    if((ident->FLAGS & USART_ISOPEN) != USART_ISOPEN) return;
    portBASE_TYPE xHigherPriorityTaskWoken = 0;
    xQueueSendToBackFromISR(ident->rx, &c, &xHigherPriorityTaskWoken);
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		uint16_t v = USART_ReceiveData(USART1);
        _irqhander(USARTa, v);
	}
}

void USART2_IRQHandler(void) {
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		uint16_t v = USART_ReceiveData(USART2);
        _irqhander(USARTb, v);
	}
}

void USART3_IRQHandler(void) {
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		uint16_t v = USART_ReceiveData(USART3);
        _irqhander(USARTc, v);
	}
}

int usart_Open(uint32_t usart_no, int flags) {

    if(usart_no >= USARTx_COUNT) { errno = ENXIO; return -1; }
    USART_Handle_t *ident = &_usarts[usart_no];

    if((ident->FLAGS & USART_ISOPEN) == USART_ISOPEN) {
        errno = EBUSY; 
        return -1;
    }

    if((ident->FLAGS & USART_ENABLED) != USART_ENABLED) {
        errno = ENXIO; 
        return -1;
    }

    ident->FLAGS |= USART_ISOPEN;

    xQueueReset(ident->rx);

    return usart_no;
}

int usart_Close(uint32_t usart_no) {
    if(usart_no >= USARTx_COUNT) { errno = ENXIO; return -1; }
    USART_Handle_t *ident = &_usarts[usart_no];

    if((ident->FLAGS & USART_ISOPEN) != USART_ISOPEN) {
        errno = EPERM; return -1;
    }
    
    ident->FLAGS &= ~(USART_ISOPEN);
    xQueueReset(ident->rx);
    return 0;
}

int usart_WriteByte(uint32_t usart_no, uint16_t v) {
    if(usart_no >= USARTx_COUNT) { errno = ENXIO; return -1; }
    USART_Handle_t *ident = &_usarts[usart_no];

    if((ident->FLAGS & USART_ISOPEN) != USART_ISOPEN) {
        errno = EPERM; return -1;
    }

    if((ident->FLAGS & USART_TX) != USART_TX) {
        errno = EPERM; return -1;
    }

    USART_SendData(ident->USARTx, v);
    while(USART_GetFlagStatus(ident->USARTx, USART_FLAG_TXE) == RESET);
    return 1;
}

int usart_ReadByte(uint32_t usart_no, uint16_t *v) {

    if(usart_no >= USARTx_COUNT) { errno = ENXIO; return -1; }
    USART_Handle_t *ident = &_usarts[usart_no];

    if((ident->FLAGS & USART_ISOPEN) != USART_ISOPEN) {
        errno = EPERM;
        return -1;
    }

    if((ident->FLAGS & USART_RX) != USART_RX) {
        errno = EPERM;
        return -1;
    }

    if(xQueueReceive( ident->rx, v, (portTickType)10 ))
        return 1;

    return 0;
}

int install_USART(uint32_t usart_no, USART_InitTypeDef *conf, uint32_t queueSize) {

    if(usart_no >= USARTx_COUNT) { errno = ENXIO; return -1; }

    USART_Handle_t *ident = &_usarts[usart_no];

    if((ident->FLAGS & USART_ENABLED) == USART_ENABLED) {
        errno = EBUSY; return -1;
    }

    // Initialize hardware
    _init_hw(usart_no, conf);

    // Setup queue
    if((ident->FLAGS & USART_RX) == USART_RX) {
        ident->rx = xQueueCreate(queueSize, sizeof(uint16_t));
    }

    return usart_no;
}
