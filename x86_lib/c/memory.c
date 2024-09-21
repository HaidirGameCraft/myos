#include <memory.h>
#include <string.h>
#include <stdio.h>

extern void _heap_start();
extern void _heap_end();

typedef struct {
    uint8_t free;
    int size;
    uint32_t next;
} block_memory;


block_memory* heap_start = (block_memory*) _heap_start;
block_memory* heap_end = (block_memory*) _heap_end;
block_memory* heap_current = (block_memory*) _heap_start;

block_memory* find_free_block( int size );
block_memory* request_new_block( int size );


void initialize_heap() {
    heap_start->free = 0;
    heap_start->size = sizeof( block_memory );
    heap_start->next = NULL;

    char s[16];
    uhtos( _heap_start, s );
    print("HEAP Start: ");
    println( s );

    uhtos( _heap_end, s );
    print("HEAP End: ");
    println( s );
}

void clear_heap() {
    memset(_heap_start, 0, _heap_end - _heap_start);
    println("[ Clear ] Clearing All Cache Memory inside of HEAP");
}

void *malloc( int size ) {
    block_memory* current = heap_start;
    block_memory* crt = heap_start;

    while( current != NULL ) {
        if( current->free == 1 && current->size >= (size + sizeof( block_memory )) ) {
            break;
        }

        crt = current;
        current = (block_memory*) current->next;
    }

    if( current != NULL ) {
        int baki = current->size - sizeof( block_memory ) - size;
        current->free = 0;
        current->size = sizeof(block_memory) + size;

        // Make split block
        if( baki > sizeof( block_memory ) ) {
            block_memory* ccrt = (block_memory*) current + current->size;
            ccrt->free = 1;
            ccrt->size = sizeof( block_memory ) + baki;
            ccrt->next = current->next;
            current->next = (uint32_t) ccrt;
        }

        return ( void * )(current + sizeof( block_memory ));
    }

    int total_size = sizeof( block_memory ) + size; // total of memory

    if( crt->size + total_size >= _heap_end ) {
        println("Memory Error> Out Of Memory.");
        return NULL;
    }

    // make new one
    current = crt + crt->size; // Get address after allocate block memory
    current->free = 0;
    current->size = sizeof( block_memory ) + size;
    current->next = NULL;

    crt->next = (uint32_t) current;

    return ( void* )(current + sizeof( block_memory ));
}

void *realloc( void* ptr, int size ) {
    block_memory* block = (block_memory*) ptr - sizeof( block_memory );

    if( block->size - sizeof( block_memory ) >= size ) {
        int baki = block->size - sizeof( block_memory ) - size;
        block->size = sizeof( block_memory ) + size;
        memcpy( block + sizeof( block_memory ), block + sizeof( block_memory ), size);

        if( baki > sizeof( block_memory ) ) {
            block_memory* ccrt = (block_memory*) block + block->size;
            ccrt->free = 1;
            ccrt->size = sizeof( block_memory ) + baki;
            ccrt->next = block->next;
            block->next = (uint32_t) ccrt;

            if( ccrt->next != NULL && ((block_memory*) ccrt->next)->free == 1 ) {
                ccrt->size += ((block_memory*) ccrt->next)->size;
                ccrt->next = ((block_memory*) ccrt->next)->next;
                ccrt->free = 1;
            }
        }

        return ( void* )( block + sizeof( block_memory ) );
    }

    
}

void free( void* ptr ) {
    block_memory* current = (block_memory*) ptr - sizeof( block_memory );
    
    if( current== NULL ) {
        println("Error Heap> Cannot reach the address because of NULL or not exist");
        return;
    }

    block_memory* next = (block_memory*) current->next;
    block_memory* prev = heap_start;

    while( prev ) {
        if( prev->next && prev->next == (uint32_t) current ) {
            break;
        }

        prev = prev->next;
    }

    // merge block when previous block and next block is null
    current->free = 1;
    if( prev->free == 1 ) {
        prev->size += current->size;
    }
    if( next->free == 1 ) {
        current->size += next->size;
    }

    if( prev->free == 0 && next->free == 0 ) {
        current->size = next - current;
    }
}

int size_memory(void* ptr) {
    block_memory* block = (block_memory*) ptr - sizeof( block_memory );
    return (int) block->size - sizeof( block_memory );
}