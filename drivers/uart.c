#include <errno.h>
#include <stdio.h>

#include <stm32f10x.h>
#include <stm32f10x_usart.h>
#include <gpio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

static struct {
	xQueueHandle txq;
	xQueueHandle rxq;
} uart;

void USART2_IRQHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = 0;

	/* Receiver has data.  Try to enqueue it (if we're unable to, we simply
	 * discard the byte and move on). */
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
		int c = USART_ReceiveData(USART2);

		xQueueSendToBackFromISR(uart.rxq, (const void *)c,
				&xHigherPriorityTaskWoken);
	}
}

int uart_getchar(void)
{
	int c;
	portBASE_TYPE res;
	
	res = xQueueReceive(uart.rxq, &c, 0);

	if (res == pdPASS)
		return c;
	else
		return EOF;
}

int uart_putchar(int c)
{
	portBASE_TYPE res;
	
	res = xQueueSendToBack(uart.txq, (const void *)c, 0);

	if (res == pdPASS)
		return c;
	else
		return EOF;
}

int uart_puts(const char *s)
{
	int count = 0;

	do {
		if (uart_putchar(*s) == EOF)
			return -EOF;
		else
			count++;
		s++;
	} while (*s);

	return count;
}

static void uart_task(void *params)
{
	int c;

	for (;;) {
		if (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != RESET) {
			xQueueReceive(uart.txq, &c, portMAX_DELAY);
			USART_SendData(USART2, c);
		} else {
			vTaskDelay(10);
		}
	}
}

static void uart_io_init(void)
{
	/* USART2 RX */
	gpio_config(GPIOA, GPIO_Pin_5,
			GPIO_Mode_AF_PP, GPIO_Speed_50MHz);
	/* USART2 TX */
	gpio_config(GPIOA, GPIO_Pin_6,
			GPIO_Mode_IN_FLOATING, GPIO_Speed_50MHz);
	/* remap for dev kit */
	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
}

static void uart_irq_init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

static void uart_hw_init(uint32_t baud)
{
	USART_InitTypeDef conf;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	conf.USART_BaudRate = baud;
	conf.USART_WordLength = USART_WordLength_8b;
	conf.USART_StopBits = USART_StopBits_1;
	conf.USART_Parity = USART_Parity_No;
	conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	conf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &conf);

	USART_Cmd(USART2, ENABLE);
}

void uart_task_init(void)
{
	uart.txq = xQueueCreate(16, sizeof(int));
	uart.rxq = xQueueCreate(16, sizeof(int));

	xTaskCreate(uart_task, (signed char *)"UART", configMINIMAL_STACK_SIZE,
			(void *)NULL, tskIDLE_PRIORITY + 3, NULL);
}

void uart_init(uint32_t baud)
{
	uart_task_init();
	uart_io_init();
	uart_hw_init(baud);
	uart_irq_init();
}
