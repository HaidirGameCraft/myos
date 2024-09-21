[BITS 32]

section .text
GLOBAL _START
_START:

    mov eax, [esp + 4]
    mov dword [vbe_address], eax

    MOV ESP, 0X00010000
    MOV EBP, ESP

    MOV AX, 0x0350
    MOV WORD [0XB8000], AX

    lgdt [ gdt_descriptor ]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    jmp 0x08:.far_protected_mode
.far_protected_mode:

    ; call intV86
    [extern kernel_main]

    mov eax, dword [vbe_address]
    push eax
    CALL kernel_main

    [extern clear_heap]
    call clear_heap
    
    HLT
    JMP $

global intV86
intV86:
    cli 
    pusha

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov eax, cr0
    and eax, 0x7FFFFFFF
    mov cr0, eax

    mov eax, cr0
    and eax, 0xFFFFFFFE
    mov cr0, eax

    jmp 0x0000:.real_mode
.real_mode:

    mov ah, 0x00
    mov al, 0x00
    int 0x10

.go_back:
    cli

    lgdt [ gdt_descriptor ]
    
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:.protected_mode
.protected_mode:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    MOV AX, 0x0352
    MOV WORD [0XB8000], AX

    popa
    sti


    ret

gdt_start:
    dd 0x00000000
    dd 0x00000000
.code32:
    dw 0xffff
    dw 0x0000
    db 0
    db 0x9a
    db 0xcf
    db 0
.data32:
    dw 0xffff
    dw 0x0000
    db 0
    db 0x92
    db 0xcf
    db 0
.code16:
    dw 0xffff
    dw 0x0000
    db 0
    db 0xfa
    db 0xcf
    db 0
.data16:
    dw 0xffff
    dw 0x0000
    db 0
    db 0xf2
    db 0xcf
    db 0
gdt_end:
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

vbe_address: resd 1