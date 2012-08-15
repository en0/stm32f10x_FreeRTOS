#include <stdio.h>
#include <stm32f10x.h>

#include <uart.h>
#include <shell.h>
#include <timer.h>

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

int main(void)
{
	timer_init();
	uart_init(115200);
	shell_init();

	vTaskStartScheduler();

	/* We'll only get here if there was insufficient memory to create the
	 * idle task. */
	for (;;);

	return 0;
}
