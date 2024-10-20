ENTRY(_START)
SECTIONS 
{
    . = 0x00100000;
    .text : {
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(COMMON)
        *(.bss)
    }
    _heap_start = 0x02000000;
    . += 0x01000000;
    _heap_end = 0x03000000;
}
