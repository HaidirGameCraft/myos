#include <page.h>
#include <kmem.h>
#include <string.h>
#include <stdio.h>
#include <cpu/isr.h>

#define PAGE_PRESENT 0x01
#define PAGE_RW 0x02

#define PAGE_DIR_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024
#define PAGE_MAP 1024
#define PAGE_SIZE 0x1000

#define __aligned__(x) __attribute__((aligned(x)))

uint32_t page_directory[1024] __aligned__(0x1000);
uint32_t page_table[1024][1024] __aligned__(0x1000);

void Page_Fault(register_t reg);

void Initialize_Paging() {
    print(" [ Initialize ] Setup Paging... \n"); 
    for(int i = 0; i < 1024; i++) {
        for(int j = 0; j < 1024; j++) {
            page_table[i][j] = ((i * 1024 + j) * PAGE_SIZE) | PAGE_READ_WRITE | PAGE_PRESENT;
        }
    }

    for(int i = 0; i < 1024; i++) {
        page_directory[i] = ((uint32_t) page_table[i]) | PAGE_PRESENT | PAGE_READ_WRITE;
    }

    __asm__ volatile("mov %0, %%cr3" :: "r"( &page_directory ));
    uint32_t cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" : : "r"( cr0 ));

    Register_Interrupt(14, Page_Fault);
    print(" [ Initialize ( DONE ) ] Setup Paging... \n");
}

void Clear_Paging() {
    print("Clearing Paging...");
}

void SetVirtualMapping(uint32_t physical_address, uint32_t virtual_address, int size) {

    uint32_t pd_index = virtual_address >> 22;
    uint32_t pt_index = ( virtual_address >> 12 ) & 0x03FF;

    page_directory[pd_index] = ((uint32_t) page_table[pd_index]) | PAGE_PRESENT | PAGE_READ_WRITE;

    int size_ = (int)(size / 4096) + 1;
    for(int i = 0; i < size_; i++) {
        page_table[pd_index][pt_index + i] = 0x00000000;
    }

    for(int i = 0; i < size_; i++) {
        page_table[pd_index][pt_index + i] = ( physical_address + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_READ_WRITE;
    }
}

void ClearVirtualMapping(uint32_t virtual_address, int size) {
    uint32_t pd_index = virtual_address >> 22;
    uint32_t pt_index = ( virtual_address >> 12 ) & 0x03FF;

    int size_ = (int)(size / 4096) + 1;
    for(int i = 0; i < size_; i++) {
        page_table[pd_index][pt_index + i] = 0x00000000;
    }
}

void Page_Fault(register_t reg) {
    uint32_t fault_addr;
    uint32_t error_code = reg.error_code;

    asm __volatile__("mov %%cr2, %0" : "=r"(fault_addr));

    printf("Fault Address: 0x%x\n Error Code: 0x%x\n", fault_addr, error_code);

    if(!(error_code & PAGE_PRESENT))
        printf("Page not Present\n");

    if( error_code & PAGE_READ_WRITE )
        printf("Write operation\n");
    else
        printf("Read operation\n");

    if( error_code & 0x4 )
        println("User mode");
    else
        println("Kernel Mode");

    if( error_code & 0x8) 
        println("reserved bit overwritting");

    if( error_code & 0x10 )
        println("Instruction fetch");

    print("Halt program");
    while(1);
    
}
