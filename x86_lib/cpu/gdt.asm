global install_gdt
install_gdt:
    mov eax, [esp + 4]
    lgdt [ eax ]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    jmp 0x08:.flush
.flush:

    mov ax, 0x28
    ltr ax

    ret