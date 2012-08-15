#include <stdio.h>
#include <stm32f10x.h>

#include <uart.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

unsigned long runtime_stats_clock = 0;

void TIM2_IRQHandler(void)
{
	runtime_stats_clock++;
}

void vApplicationTickHook(void)
{
}

static void timer_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

int main(void)
{
	timer_init();
	uart_init(115200);

	vTaskStartScheduler();

	/* We'll only get here if there was insufficient memory to create the
	 * idle task. */
	for (;;) {
		int c;

		c = uart_getchar();
		if (c > 0)
			uart_putchar(c);
	}

	return 0;
}
