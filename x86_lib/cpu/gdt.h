#ifndef __GDT__
#define __GDT__

#include <stdint.h>

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t flags;
    uint8_t base_high;
} __attribute__((packed)) GDT_Entry;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDT_Descriptor;

typedef struct {
    uint32_t link;
    uint32_t esp0, ss0;
    uint32_t esp1, ss1;
    uint32_t esp2, ss2;
    uint32_t cr3, eip, eflags;
    uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
    uint32_t es, cs, ss, ds, fs, gs, idtr, iopb;
} __attribute__((packed)) TSS_Segment;

extern void install_gdt(uint32_t addr);
void Initialize_GDT();
void SetGDTGate(int index, uint32_t limit, uint32_t base, uint8_t access, uint8_t flags);

#endif