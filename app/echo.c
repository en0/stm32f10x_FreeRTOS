#include <FreeRTOS.h>
#include <uart.h>
#include <echo.h>
#include <task.h>

#include <stdio.h>
#include <device.h>

void send_string(const char* str);

void _greet(FILE* f) {
    char name[20];
    int i; 
    char c;

    fprintf(f, "What is your name? ");

    for(i = 0; i < 20; i++) {
        c = fgetc(f);
        if(c == '\r') { name[i] = '\0'; break; }
        name[i] = c;
        fputc(c, f); fflush(f);
    }

    fprintf(f, "\r\nHello, %s\r\n", name);
}

void _main(void* pvParams) {

    FILE *f = fopen("/dev/ttyUART2", "r+");
    if(f == NULL) for(;;);

    fputs("A simple echo program\r\n"
          "---------------------\r\n", f);

    _greet(f);

    char c; 
    while(1) {
        c = fgetc(f);
        if(c == '\r') fputs("\r\n",f);
        fputc(c, f); fflush(f); // just echo
    }
    fclose(f);
}


void echo_app() {

    xTaskCreate(
        _main, 
        (const signed char*)"ECHO", 
        configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL
    );

    
}
