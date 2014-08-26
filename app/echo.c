#include <FreeRTOS.h>
#include <task.h>
#include <echo.h>
#include <stdio.h>

void echo_main(void* pvParams) {

    char c;
    while(1) {
        c = getchar();
        putchar(c);
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
