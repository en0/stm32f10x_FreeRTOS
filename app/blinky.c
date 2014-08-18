#include <FreeRTOS.h>
#include <task.h>
#include <gpio.h>
#include <blinky.h>
#include <task.h>

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
        vTaskDelay(500);
    }
}

xTaskHandle* blinky_app() {
    xTaskHandle xHandle = NULL;
    _init_led();
    xTaskCreate(blinky, (const signed char*)"Blinky", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xHandle);
    configASSERT(xHandle);
    return xHandle;
}
