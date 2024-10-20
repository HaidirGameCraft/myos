OUTPUT_FORMAT(binary)
ENTRY(main)
SECTIONS 
{
    . = 0x08000000;
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
    _heap_start = .;
    . += 0x001000000;
    _heap_end = .;
}
