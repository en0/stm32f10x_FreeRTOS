#include <FreeRTOS.h>
#include <gpio.h>
#include <blinky.h>
#include <task.h>

void _delay(uint32_t delay_count) {
    for(;delay_count != 0; delay_count--);
}

void _init_led() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    gpio_config(
        GPIOA, 
        GPIO_Pin_0, 
        GPIO_Mode_Out_PP, 
        GPIO_Speed_50MHz
    );
}

void blinky(void *params) {
    while(1) {
        GPIOA->ODR ^= GPIO_Pin_0;
        _delay(500000);
    }
}

xTaskHandle* blinky_app() {
    xTaskHandle xHandle = NULL;
    _init_led();
    xTaskCreate(blinky, (const signed char*)"Blinky", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle);
    configASSERT(xHandle);
    return xHandle;
    //if(xHandle != NULL) {
    //}
}
