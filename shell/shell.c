#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <uart.h>
#include <shell.h>

static void shell_task(void *params)
{
	uart_puts(WELCOME_STRING);

	while (1) {
		int c;
		
		c = uart_getchar();
		if (c != EOF) {
		}
	}
}

void shell_init(void)
{
	xTaskCreate(shell_task, (signed char *)"SHELL",
			configMINIMAL_STACK_SIZE, (void *)NULL,
			tskIDLE_PRIORITY + 4, NULL);
}
