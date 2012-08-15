#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <uart.h>
#include <shell.h>

static struct {
	char buf[SHELL_CMD_BUFFER_LEN];
	unsigned int index;
	unsigned int match;
} shell;

static void prompt(void)
{
	uart_puts(SHELL_PROMPT_STRING);
}

static void parse_command(void)
{
}

static void handle_input(char c)
{
	switch (c) {
		case '\r':
		case '\n':
			uart_putchar('\n');
			if (shell.index > 0) {
				parse_command();
				shell.index = 0;
				memset(shell.buf, 0, sizeof(shell.buf));
			}
			prompt();
	}
}

static void shell_task(void *params)
{
	uart_puts(SHELL_WELCOME_STRING);
	prompt();

	while (1) {
		int c;
		
		c = uart_getchar();
		if (c != EOF) {
			handle_input((char)c);
		}
	}
}

void shell_init(void)
{
	xTaskCreate(shell_task, (signed char *)"SHELL",
			configMINIMAL_STACK_SIZE, (void *)NULL,
			tskIDLE_PRIORITY + 4, NULL);
}
