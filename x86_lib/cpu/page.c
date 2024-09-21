#include <page.h>
#include <stdio.h>

#define PAGE_PRESENT 0x01
#define PAGE_RW 0x02

#define PAGE_DIR_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024
#define PAGE_SIZE 0x1000

#define __aligned__(x) __attribute__((aligned(x)))

uint32_t page_directory[ PAGE_DIR_ENTRIES ] __aligned__(( PAGE_SIZE ));
uint32_t page_table[ PAGE_TABLE_ENTRIES ][ 1024 ] __aligned__(( PAGE_SIZE ));

void Initialize_Paging() {
    print(" [ Initialize ] Setup Paging... \n");
    for( int i = 0; i < PAGE_DIR_ENTRIES; i++) {
        page_directory[i] = ((uint32_t) page_table[i] | PAGE_RW | PAGE_PRESENT );
    }

    for(int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        for(int j = 0; j < PAGE_TABLE_ENTRIES; j++) {
            page_table[i][j] = ((i * PAGE_TABLE_ENTRIES + j) * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;
        }
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"( &page_directory ));

    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" : : "r"( cr0 ));

    print(" [ Initialize ( DONE ) ] Setup Paging... \n");
}

void SetVirtualMapping(uint32_t physical_address, uint32_t virtual_address) {
    page_directory[ virtual_address >> 0x16 ] = ((uint32_t) page_table[ virtual_address >> 0x16 ]) | PAGE_PRESENT | PAGE_RW;

    for(int i = 0; i < 256; i++) {
        page_table[ virtual_address >> 0x16 ][i] = ( physical_address + i * PAGE_SIZE ) | PAGE_PRESENT | PAGE_RW;
    }
}