#ifndef __GPIO_H__
#define __GPIO_H__

#include <stm32f10x_gpio.h>

void gpio_config(GPIO_TypeDef *port, uint16_t pin_mask, GPIOMode_TypeDef mode,
		GPIOSpeed_TypeDef speed);

#endif /* __GPIO_H__ */
