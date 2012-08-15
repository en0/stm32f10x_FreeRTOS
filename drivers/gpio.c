#include <stm32f10x.h>
#include <gpio.h>

void gpio_config(GPIO_TypeDef *port, uint16_t pin_mask, GPIOMode_TypeDef mode,
		GPIOSpeed_TypeDef speed)
{
	GPIO_InitTypeDef conf;

	conf.GPIO_Pin = pin_mask;
	conf.GPIO_Mode = mode;
	conf.GPIO_Speed = speed;

	GPIO_Init(port, &conf);
}
