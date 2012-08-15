#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <uart.h>
#include <shell.h>

#define ARRAY_SIZE(a)	(sizeof(a)/sizeof(a[0]))

static struct {
	char buf[SHELL_CMD_BUFFER_LEN];
	unsigned int index;
	unsigned int match;
} shell;

/* built-in shell commands */
static void shell_help(int argc, char **argv);

static struct shell_command commands[] = {
	{ "help",	shell_help },
};

static void prompt(void)
{
	uart_puts(SHELL_PROMPT_STRING);
}

static char is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static void do_command(int argc, char **argv)
{
	unsigned int i;
	unsigned int nl = strlen(argv[0]);
	unsigned int cl;

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		cl = strlen(commands[i].name);

		if (cl == nl && commands[i].function != NULL &&
				!strncmp(argv[0], commands[i].name, nl)) {
			commands[i].function(argc, argv);
			uart_putchar('\n');
		}
	}
}

static void parse_command(void)
{
	unsigned char i;
	char *argv[16];
	int argc = 0;
	char *in_arg = NULL;

	for (i = 0; i < shell.index; i++) {
		if (is_whitespace(shell.buf[i]) && argc == 0)
			continue;

		if (is_whitespace(shell.buf[i])) {
			if (in_arg) {
				shell.buf[i] = '\0';
				in_arg = NULL;
			}
		} else if (!in_arg) {
			in_arg = &shell.buf[i];
			argv[argc] = in_arg;
			argc++;
		}
	}
	shell.buf[i] = '\0';

	if (argc > 0)
		do_command(argc, argv);
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
	memset(&shell, 0, sizeof(shell));

	xTaskCreate(shell_task, (signed char *)"SHELL",
			configMINIMAL_STACK_SIZE, (void *)NULL,
			tskIDLE_PRIORITY + 4, NULL);
}

static void shell_help(int argc, char **argv)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(commands); i++) {
		uart_puts(commands[i].name);
		uart_putchar('\n');
	}
}
