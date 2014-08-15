#include <FreeRTOS.h>
#include <timer.h>
#include <task.h>
#include <device.h>

// Application Includes
#include <blinky.h>
#include <echo.h>

/** Some debugging Stuff **/
#include <stdio.h>
#include <uart.h>
void send_byte(uint8_t b) {

    switch(b) {
        case '\n':
            send_byte('\r');
            USART_SendData(USART2, '\n');
            while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
            break;

        default:
            USART_SendData(USART2, b);
            break;

    }
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void send_string(const char* str) {
    // For debugging
    for(;*str!='\0'; str++)
        send_byte(*str);
}

void vApplicationTickHook(void) {
    /**
     ** Fires on timer tick
     **/
}

void vApplicationStackOverflowHook(void) {
    /**
     ** Fires on stack overflow
     **/

    send_byte('!');
}

int main(void)
{
    //extern char _end;
    //static char *heap_end;

    init_wireup();  // Configure wireup
	timer_init();   // Start timers
    blinky_app();   // Status Application
    echo_app();     // Main Program


    /*
    FILE *f = fopen("/dev/ttyUART2", "w");
    if(f == NULL) for(;;);
    fputs("Testing printf functions\r\n\r\n", f);

    printf("Pointer to this UART: %p\r\n", &f);
    printf("Pointer to bottom of heap: %p\r\n", &heap_end);
    printf("Pointer to bottom of image: %p\r\n", &_end);

    fputs("\r\nTest Complete\r\n", f);
    */

	vTaskStartScheduler();

	/** We'll only get here if there was insufficient memory    **
	 ** to create the idle task.                                **/

    send_string("Insufficient meory to create idle task!\n");

    for(;;);

	return 0;
}
