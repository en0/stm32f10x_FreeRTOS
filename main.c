#include <blinky.h>
#include <timer.h>


void vApplicationTickHook(void)
{
}

void vApplicationStackOverflowHook(void)
{
}

int main(void)
{
	timer_init();
    blinky_app();
	vTaskStartScheduler();

	/* We'll only get here if there was insufficient memory to create the
	 * idle task. */
    for(;;);

	return 0;
}
