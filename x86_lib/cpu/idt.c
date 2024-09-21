#include "idt.h"
#include <driver/port.h>

IDT_ENTRY idt_entries[256];
IDT_REGISTER idt_ptr;

void Initialize_IDT() {
    idt_ptr.limit = sizeof( idt_entries ) - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    Install_IDT((uint32_t) &idt_ptr);
}

void Set_IDT_Gate(uint8_t index, uint32_t handle, uint8_t flags) {
    idt_entries[index].low_offset = handle & 0xFFFF;
    idt_entries[index].high_offset = (handle >> 16) & 0xFFFF;
    idt_entries[index].kernel_cs = 0x08;
    idt_entries[index].reserved = 0;
    idt_entries[index].attributes = flags;
}