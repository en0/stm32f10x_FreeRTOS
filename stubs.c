#include <stdint.h>


/** Some really nasty heap allocation **/
void* _sbrk( uint32_t x )
{
    extern char _end;
    static char *heap_end;
    char *prev_heap_end;

    if(heap_end == 0)
        heap_end = &_end;

    prev_heap_end = heap_end;
    heap_end += x;

    return (void*)prev_heap_end;
}

void _exit(uint32_t status) {
    for(;;);
}
