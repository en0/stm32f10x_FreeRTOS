#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

static void shell_task(void *params)
{
	while (1) {
	}
}

void shell_init(void)
{
	xTaskCreate(shell_task, (signed char *)"SHELL",
			configMINIMAL_STACK_SIZE, (void *)NULL,
			tskIDLE_PRIORITY + 4, NULL);
}
