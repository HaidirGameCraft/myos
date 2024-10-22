#ifndef __KERNEL_MEMORY__
#define __KERNEL_MEMORY__

#include <stdint.h>

void kheap_initialize(uint32_t addr_heap_start, uint32_t addr_heap_end); 
void kheap_clear();
void* kmalloc( int size );
void kfree( void* ptr );

#define CREATE_ALLOC(type, size)( (type) kmalloc(size) )

#endif
