#include <FreeRTOS.h>
#include <timer.h>
#include <task.h>

// Wireup Proto
extern void init_wireup();

// Application Includes
#include <blinky.h>

/** Some debugging Stuff **/
#include <stdio.h>
#include <string.h>
#include <usart.h>
void send_byte(uint8_t b) {

    switch(b) {
        case '\n':
            send_byte('\r');
            USART_SendData(USART3, '\n');
            while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
            break;

        default:
            USART_SendData(USART3, b);
            break;

    }
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void send_string(const char* str) {
    // For debugging
    for(;*str!='\0'; str++)
        send_byte(*str);
}

void send_memory(void* ptr, size_t len) {
    char* x = (char*)ptr;
    char buffer[100];
    char buffer2[50];
    int i;

    sprintf(buffer, "Dumping Memory at [0x%x]\n[ %08X ] ", (int)x, (int)x);
        send_string(buffer);

    for(i = 1; len > 0; len--, x++, i++) {
        sprintf(buffer, "%02X ", *x);
        if((i%16) == 0) {
            sprintf(buffer2, "\n[ %08X ] ", (int)x);
            strcat(buffer, buffer2);
        }
        else if((i%8) == 0) strcat(buffer, "- ");
        send_string(buffer);
    }
    send_byte('\n');
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

	timer_init();   // Start timers
    init_wireup();  // Configure wireup

    // Start Services
    blinky_app();
    //echo_app();

    send_string("** NOTICE: [ Test drivers ]\n");


    printf("Hello, World!\r\n");

    send_string("** NOTICE: [ Starting Task Scheduler ]\n");

	vTaskStartScheduler();

	/** We'll only get here if there was insufficient memory    **
	 ** to create the idle task.                                **/

    send_string("Insufficient meory to create idle task!\n");

    for(;;);

	return 0;
}
