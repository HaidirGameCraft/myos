#ifndef _IDT_HEADER_
#define _IDT_HEADER_

#include <stdint.h>

typedef struct {
    uint16_t low_offset;
    uint16_t kernel_cs;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t high_offset;
} __attribute__((packed)) IDT_ENTRY;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDT_REGISTER;

extern void Install_IDT(uint32_t);


void Initialize_IDT();
void Set_IDT_Gate(uint8_t index, uint32_t handle, uint8_t flags);

#endif