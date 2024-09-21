#include "gdt.h"

GDT_Entry entries[6];
GDT_Descriptor gdt_ptr;
TSS_Segment tss_segment;

void Initialize_GDT() {

    SetGDTGate( 0, 0x00000000, 0x00000000, 0x00, 0x00);
    SetGDTGate( 1, 0xFFFFFFFF, 0x00000000, 0x9A, 0xCF);
    SetGDTGate( 2, 0xFFFFFFFF, 0x00000000, 0x92, 0xCF);
    SetGDTGate( 3, 0xFFFFFFFF, 0x00000000, 0xFA, 0xCF);
    SetGDTGate( 4, 0xFFFFFFFF, 0x00000000, 0xF2, 0xCF);
    SetGDTGate( 5, sizeof( TSS_Segment ) - 1, &tss_segment, 0x89, 0);

    memset(&tss_segment, 0, sizeof( TSS_Segment ));
    tss_segment.ss0 = 0x10;
    tss_segment.esp0 = 0x00100000;

    gdt_ptr.limit = sizeof( GDT_Entry ) * 6 - 1;
    gdt_ptr.base = (uint32_t) entries;

    install_gdt( (uint32_t) &gdt_ptr );
}

void SetGDTGate(int index, uint32_t limit, uint32_t base, uint8_t access, uint8_t flags) {
    entries[index].limit_low = limit & 0xFFFF;
    entries[index].base_low = base & 0xFFFF;
    entries[index].base_mid = ( base >> 16 ) & 0xFF;
    entries[index].access = access;
    entries[index].flags = flags & 0xF0;
    entries[index].flags |= ( limit >> 16 ) & 0x0F;
    entries[index].base_high = ( base >> 24 ) & 0xFF;

}