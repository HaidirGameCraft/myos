#ifndef __MEMORY_HEADER__
#define __MEMORY_HEADER__

#include <stdint.h>

void initialize_heap();
void clear_heap();

void *malloc( int size );
void *realloc( void* ptr, int size );
void free( void* ptr );

int size_memory(void* ptr);

#endif