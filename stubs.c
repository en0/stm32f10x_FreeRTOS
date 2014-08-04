#include <stdint.h>
#define NULL (void*)0x00
char * _sbrk( uint32_t x )
{
    /* Just to remove compiler warning. */
    ( void ) x;
    return NULL;
}

void _exit(uint32_t status) {
    for(;;);
}
