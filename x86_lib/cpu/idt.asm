section .text
GLOBAL Install_IDT
Install_IDT:
    MOV EAX, [ESP + 4]
    LIDT [EAX]
    STI
    RET

; ISR Stub
ISR_STUB:
    PUSHA
    MOV EAX, DS
    PUSH EAX
    MOV AX, 0X10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX

[EXTERN ISR_Handler]
    CALL ISR_Handler

    POP EAX
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    POPA
    ADD ESP, 8
    STI
    IRET


global syscall_asm
syscall_asm:
    pusha

    mov eax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

[extern syscall_handler]
    call syscall_handler
    
    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
;    mov eax, store_eax
   iret

IRQ_STUB:
    PUSHA
    MOV EAX, DS
    PUSH EAX
    MOV AX, 0X10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX

[EXTERN IRQ_Handler]
    CALL IRQ_Handler
    
    POP EAX
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    POPA
    ADD ESP, 8
    STI
    IRET



%macro ISR 2
GLOBAL ISR%1
ISR%1:
    CLI
    PUSH BYTE %2
    PUSH BYTE %1
    JMP ISR_STUB
%endmacro

%macro IRQ 2
GLOBAL IRQ%1
IRQ%1:
    CLI
    PUSH BYTE %2
    PUSH BYTE %1
    JMP IRQ_STUB
%endmacro

ISR 0, 0
ISR 1, 0
ISR 2, 0
ISR 3, 0
ISR 4, 0
ISR 5, 0
ISR 6, 0
ISR 7, 0
ISR 8, 0
ISR 9, 0
ISR 10, 0
ISR 11, 0
ISR 12, 0
ISR 13, 0
ISR 14, 0
ISR 15, 0
ISR 16, 0
ISR 17, 0
ISR 18, 0
ISR 19, 0
ISR 20, 0
ISR 21, 0
ISR 22, 0
ISR 23, 0
ISR 24, 0
ISR 25, 0
ISR 26, 0
ISR 27, 0
ISR 28, 0
ISR 29, 0
ISR 30, 0
ISR 31, 0

IRQ 32, 0
IRQ 33, 1
IRQ 34, 2
IRQ 35, 3
IRQ 36, 4
IRQ 37, 5
IRQ 38, 6
IRQ 39, 7
IRQ 40, 8
IRQ 41, 9
IRQ 42, 10
IRQ 43, 11
IRQ 44, 12
IRQ 45, 13
IRQ 46, 14
IRQ 47, 15

section .data
store_eax: dd 0
