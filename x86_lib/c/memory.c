#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <page.h>

#define HEAP_START 0x02000000
#define HEAP_END 0x03000000

typedef struct {
    uint8_t free;
    int size;
    uint32_t next;
} block_memory;


block_memory* heap_start = (block_memory*) HEAP_START;
block_memory* heap_end = (block_memory*) HEAP_END;
block_memory* heap_current = (block_memory*) HEAP_START;

block_memory* find_free_block( int size );
block_memory* request_new_block( int size );


void initialize_heap() {
    memset(HEAP_START, 0, HEAP_END - HEAP_START);
    heap_start->free = 0;
    heap_start->size = sizeof( block_memory );
    heap_start->next = NULL;

    char *s = uhtos( HEAP_START );
    print("HEAP Start: ");
    println( s );
    free(s);

    s = uhtos( HEAP_END );
    print("HEAP End: ");
    println( s );
    free(s);
}

void clear_heap() {
    memset(HEAP_START, 0, HEAP_END - HEAP_START);
    println("[ Clear ] Clearing All Cache Memory inside of HEAP");
}

void *malloc( int size ) {
    block_memory* current = heap_start;
    block_memory* crt = heap_start;

    while( current != NULL ) {
        //printf("Check\n");
        if( current->free == 1 && current->size >= (size + sizeof( block_memory )) ) {
            break;
        }

        crt = current;
        current = (block_memory*) current->next;
    }

    if( current == heap_end )
    {
        printf("End of Memory\n");
        return NULL;
    }

    //printf("2.Testing OK...\n");

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

      //  printf("3.Testing OK...\n");

        return ( void * )(current + sizeof( block_memory ));
    }

    int total_size = sizeof( block_memory ) + size; // total of memory

    //printf("4.Testing OK...\n");

    if( crt->size + total_size >= HEAP_END ) {
        println("Memory Error> Out Of Memory.");
        return NULL;
    }

    //printf("5.Testing OK...\n");

    // make new one
    current = crt + crt->size; // Get address after allocate block memory
    current->free = 0;
    current->size = sizeof( block_memory ) + size;
    current->next = NULL;

    crt->next = (uint32_t) current;

    return ( void* )(current + sizeof( block_memory ));
}

void *realloc( void* ptr, int size ) {
    block_memory* block = (block_memory*)( ptr - sizeof( block_memory ) );

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
    } else {
        void* new_alloc = malloc( size );
        memcpy(new_alloc, ptr, block->size - sizeof(block_memory));
        free(ptr);
        return new_alloc;
    }
}

void free( void* ptr ) {
    if( ptr == NULL ) return;
    
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

        if( current->next != NULL )
            prev->next = current->next;
        else
            prev->next = NULL;

    }

    // char s[16];
    // s_uhtos( next->free, s );
    // print(s);
    // print("\n");

    if( next == NULL ) {
        current->next = NULL;
    } else if( next ) {
        if( next->free == 1 ) {
            current->size += next->size;

            if( next->next != NULL )
                current->next = next->next;
            else
                current->next = NULL;
        } else if( next->free == 0) {
            prev->next = next;
        }
    }

    // if( prev->free == 0 && next->free == 0 ) {
    //     current->size = next - current;
    // }
}

int size_memory(void* ptr) {
    block_memory* block = (block_memory*) ptr - sizeof( block_memory );
    return (int) block->size - sizeof( block_memory );
}
