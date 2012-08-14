#include <stdio.h>
#include <stm32f10x.h>

/* Scheduler includes. */
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

unsigned long ulRunTimeStatsClock = 0;

void TIM2_IRQHandler(void)
{
  ulRunTimeStatsClock++;
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

  for (;;);

  return 0;
}
