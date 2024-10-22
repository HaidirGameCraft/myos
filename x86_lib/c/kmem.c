#include "string.h"
#include <kmem.h>
#include <stdint.h>
#include <stdio.h>

#define BLOCK_ATTR_FREE     1
#define BLOCK_ATTR_USE      2

typedef struct {
    uint8_t inuse;
    int size;
    uint32_t next;
} block_mem_t;

#define BLOCK_MEM_SIZE sizeof(block_mem_t)

block_mem_t* find_free_block( int size );
block_mem_t* new_heap_block( int size );

block_mem_t* kheap_start = NULL;
block_mem_t* kheap_end = NULL;

void kheap_initialize(uint32_t addr_heap_start, uint32_t addr_heap_end) {
    kheap_start = (block_mem_t*) addr_heap_start;
    kheap_end = (block_mem_t*) addr_heap_end;
//    memempty(kheap_start, addr_heap_end - addr_heap_start);

    kheap_start->inuse = 0;
    kheap_start->size = addr_heap_end - addr_heap_start;
    kheap_start->next = NULL;
}

void kheap_clear() {
    memset(kheap_start, 0, kheap_end - kheap_start);
}

void *kmalloc(int size) {
    if( size <= 0 ) {
        return NULL;
    }

    block_mem_t* block = find_free_block(size);
    // when heap block is not NULL and is not use
    if( block ) {
        block->inuse = 1;
        block->size = BLOCK_MEM_SIZE + size; 
        return (void*)( block + BLOCK_MEM_SIZE );
    }

    // when heap block is NULL we need to create new heap block
    block = new_heap_block(size);

    if( block != NULL ) {
        // when new block has been created
        block->inuse = 1;
        return (void*)(block + BLOCK_MEM_SIZE);
    }

    while( 1 ) {
        printf("Stopped because there have error on kmalloc\n");
        asm volatile("hlt");
    }

    // if heap block is NULL so we need to return NULL because it not have any block to create 
    return NULL;
}

void kfree(void *ptr) {
    if( ptr == NULL ) {
        return;
    }

    block_mem_t* block = (block_mem_t*) ptr - BLOCK_MEM_SIZE;
    block->inuse = 0;
    //printf("Siz Malloc: %i\n", block->size);
    //memcpy(block, block, BLOCK_MEM_SIZE);

    block_mem_t* next_block = (block_mem_t*) block->next;
    block_mem_t* current = (block_mem_t*) kheap_start;
    block_mem_t* prev_block = (block_mem_t*) kheap_start;

    // find previous block
    //printf("Free Heap Address: 0x%x\n", block);
    while( current ) {
        //printf("Prev Block: 0x%x\n", prev_block);

        if( current == block ) {
            break;
        }
        
        if( current < kheap_start || current > kheap_end ) {
            printf("Out Of Bound Heap\n");
            while(1)
                asm volatile("hlt");
            
        }
        prev_block = current;
        current = (block_mem_t*) current->next;
    }

    if( prev_block != block ) {
        if( next_block != NULL ) {
            prev_block->next = (uint32_t) next_block;
        } else {
            prev_block->next = NULL;
        }     
    }
   
}


block_mem_t* find_free_block(int size) {
   block_mem_t* current = kheap_start;
   while( current != NULL ) {
        //printf("Find Free Block 0x%x, Block in use %i, Next 0x%x\n Size Free Block: %i Compare size with: %i\n",current, current->inuse, current->next, current->size, size + BLOCK_MEM_SIZE);
        if( current->inuse == 0 && current->size >= size + BLOCK_MEM_SIZE ) {
            return current;
        }

        block_mem_t* next = (block_mem_t*) current->next;
        if( current->next != NULL ) {
            int size_unused = next - current - current->size;
            //printf("Size Unused: %i\n Start Heap: 0x%x, Next Heap: 0x%x\n", 
            //        size_unused, current, next);
            if( current->inuse == 1 && size_unused > size + BLOCK_MEM_SIZE) {
                block_mem_t* tmp = current;
                current += current->size;
                current->size = size + BLOCK_MEM_SIZE;
                current->inuse = 0;
                current->next = (uint32_t) next;
                tmp->next = (uint32_t) current;
                return current;
            }
        }
//        printf("Current Address on find_free_block: 0x%x with size: %i\n", current, current->attr);

        current = (block_mem_t*) current->next;
   }

   return NULL;
}

block_mem_t* new_heap_block( int size ) {
    block_mem_t* current = kheap_start;

    // Find the NULL next block address
    while( current->next != NULL ) {
        if( current->inuse == 1 && current->next == NULL ) {
            break;
        }
    
        current = (block_mem_t*) current->next;
    }

//    printf("Address: 0x%x\n size: %i\n compare with: %i\n", current, size + BLOCK_MEM_SIZE, kheap_end - current - current->size);

    if( size + BLOCK_MEM_SIZE >= kheap_end - current - current->size  ) {
        printf("ERR KMEM> Out Of Memory\n");
        return NULL;
    }

    block_mem_t* tmp_block = current;

    current += current->size;
    current->size = BLOCK_MEM_SIZE + size;
    current->inuse = 0;
    current->next = NULL;

    tmp_block->next = (uint32_t) current;
    //printf("Current Address: 0x%x, Next Address: 0x%x with size %i\n", tmp_block, current, current->size);

    return current;
}
