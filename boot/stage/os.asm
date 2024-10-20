[BITS 16]

section .data

global vesa_info_addr
vesa_info_addr:
    resb 512

global vesa_mode_addr
vesa_mode_addr:
    resb 91 + 256

section .text
jmp short os_stage
nop

id_vesa: equ 0x0115

global os_stage
os_stage:

    mov sp, 0x1000
    mov bp, sp
    ; select vesa mode
    mov ax, 0x4f02
    mov bx, id_vesa
    or bx, 0x4000
    int 0x10

    ; get vesa info block
    mov ax, 0x4f00
    mov di, vesa_info_addr
    int 0x10

    ; get vesa mode info block
    mov ax, 0x4f01
    mov cx, 0x0115
    mov di, vesa_mode_addr
    int 0x10

    call enable_a20

    ; load gdt to switch from real mode to protected mode
    jmp load_gdt

enable_a20:
    cli

    call .a20wait
    mov al, 0xad
    out 0x64, al

    call .a20wait 
    mov al, 0xd0
    out 0x64, al

    call .a20wait2 
    in al, 0x60
    push eax

    call .a20wait 
    mov al, 0xd1
    out 0x64, al

    call .a20wait 
    pop eax
    or al, 2
    out 0x60, al

    call .a20wait 
    mov al, 0xae
    out 0x64, al

    call .a20wait
    sti
    ret

.a20wait:
    in al, 0x64
    test al, 2
    jnz .a20wait
    ret
.a20wait2:
    in al, 0x64
    test al, 1
    jz .a20wait2
    ret

load_gdt:
    cli
    lea eax, [GDT_DESCRIPTOR]
    lgdt [eax]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:protected_mode

%include "boot/stage/gdt.asm"


[bits 32]
[extern main]
[extern print]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax


    mov esp, 0x90000
    mov ebp, esp

    call VGA_CLEARSCREEN


    push dword test_text
    call print
    add esp, 4

    push dword installing_idt
    call print
    add esp, 4
    
    call main

    hlt
    jmp $

section .data
test_text: db "hello, world....", 0xa, 0
installing_idt : db "[install] idt...", 0xa, 0
install_idt_done: db "[done] installing idt...", 0xa, 0

%include "boot/stage/vga.asm"

