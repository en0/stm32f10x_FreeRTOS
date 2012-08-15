#include <stdint.h>
#include <stm32f10x.h>
#include <stm32f10x_tim.h>

#include <FreeRTOS.h>

#define TIMER_INTERRUPT_FREQUENCY	((uint16_t)20000)

/* The expected time between each of the timer interrupts - if the jitter was
 * zero. */
#define TIMER_EXPECTED_DIFFERENCE_VALUE	(configCPU_CLOCK_HZ / TIMER_INTERRUPT_FREQUENCY)

/* The number of interrupts to pass before we start looking at the jitter. */
#define TIMER_SETTLE_TIME		5

/* Stores the value of the maximum recorded jitter between interrupts. */
volatile unsigned short max_jitter_us = 0;

/* Variable that counts at 20KHz to provide the time base for the run time
 * stats. */
unsigned long runtime_stats_clock = 0UL;

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

	freq = configCPU_CLOCK_HZ / TIMER_INTERRUPT_FREQUENCY;

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

void TIM2_IRQHandler( void )
{
	static uint16_t last_count_us = 0;
	static uint16_t settle_count_us = 0;
	static uint16_t max_difference_us = 0;

	uint16_t this_count_us, difference_us;

	/* Capture the free running TIM3 value as we enter the interrupt. */
	this_count_us = TIM3->CNT;

	if (settle_count_us >= TIMER_SETTLE_TIME) {
		/* What is the difference between the timer value in this
		 * interrupt and the value from the last interrupt? */
		difference_us = this_count_us - last_count_us;

		/* Store the difference in the timer values if it is larger
		 * than the currently stored largest value.  The difference
		 * over and above the expected difference will give the
		 * 'jitter' in the processing of these interrupts. */
		if (difference_us > max_difference_us) {
			max_difference_us = difference_us;
			max_jitter_us = max_difference_us -
				TIMER_EXPECTED_DIFFERENCE_VALUE;
		}
	} else {
		/* Don't bother storing any values for the first couple of
		 * interrupts. */
		settle_count_us++;
	}

	/* Remember what the timer value was this time through, so we can
	 * calculate the difference the next time through. */
	last_count_us = this_count_us;

	/* Keep a count of the number of interrupts as a time base for the run
	 * time stats collection. */
	runtime_stats_clock++;

	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}

