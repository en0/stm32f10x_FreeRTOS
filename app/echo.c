#include <FreeRTOS.h>
#include <task.h>
#include <echo.h>

void echo_main(void* pvParams) {

    while(1) {
        vTaskDelay(1000);
    }
}


void echo_app() {

    xTaskCreate(
        echo_main, 
        (const signed char*)"ECHO", 
        configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL
    );

    
}
