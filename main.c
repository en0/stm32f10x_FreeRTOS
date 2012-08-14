#include <stdio.h>
#include <stm32f10x.h>

/* Scheduler includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

unsigned long runtime_stats_clock = 0;

void TIM2_IRQHandler(void)
{
	runtime_stats_clock++;
}

void vApplicationTickHook(void)
{
}

static void clock_init(void)
{
}

static void timer_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
}

int main(void)
{
	clock_init();
	timer_init();

	vTaskStartScheduler();

	/* We'll only get here if there was insufficient memory to create the
	 * idle task. */
	for (;;);

	return 0;
}
