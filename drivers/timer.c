#include <stdint.h>
#include <stm32f10x.h>
#include <stm32f10x_tim.h>

#include <FreeRTOS.h>

#define timerINTERRUPT_FREQUENCY                ((uint16_t)20000)

static void timer_clock_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

static void timer_hw_init(void)
{
	TIM_TimeBaseInitTypeDef conf;
	unsigned long freq;

	TIM_DeInit(TIM2);
	TIM_DeInit(TIM3);

	/* TIM2 for generating interrupts */

	freq = configCPU_CLOCK_HZ / timerINTERRUPT_FREQUENCY;
	
	conf.TIM_Period = (uint16_t)(freq & 0xFFFF);
	conf.TIM_Prescaler = 0;
	conf.TIM_ClockDivision = 0;
	conf.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &conf);
	TIM_ARRPreloadConfig(TIM2, ENABLE);

	/* TIM3 for high resolution time measurement */
	
	conf.TIM_Period = 0xFFFF;
	TIM_TimeBaseInit(TIM3, &conf);
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}

static void timer_irq_init(void)
{
	NVIC_InitTypeDef conf;

	conf.NVIC_IRQChannel = TIM2_IRQn;
	conf.NVIC_IRQChannelSubPriority = 0;
	conf.NVIC_IRQChannelPreemptionPriority = 0;
	conf.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&conf);
}

void timer_init(void)
{
	timer_clock_init();
	timer_irq_init();
	timer_hw_init();
}
