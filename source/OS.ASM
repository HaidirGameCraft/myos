[BITS 16]

section .data
global vesa_addr
vesa_addr:
    resb 91 + 256

section .text
JMP SHORT OS_STAGE
NOP

GLOBAL OS_STAGE
OS_STAGE:

    MOV SP, 0X1000
    MOV BP, BP

    mov ax, 0x4f02
    mov bx, 0x115 | 0x4000
    int 0x10

    mov ax, 0x4f01
    mov cx, 0x115
    mov di, vesa_addr
    int 0x10


    CALL ENABLE_A20

    JMP LOAD_GDT

ENABLE_A20:
    CLI

    CALL .A20WAIT
    MOV AL, 0XAD
    OUT 0X64, AL

    CALL .A20WAIT 
    MOV AL, 0XD0
    OUT 0X64, AL

    CALL .A20WAIT2 
    IN AL, 0X60
    PUSH EAX

    CALL .A20WAIT 
    MOV AL, 0XD1
    OUT 0X64, AL

    CALL .A20WAIT 
    POP EAX
    OR AL, 2
    OUT 0X60, AL

    CALL .A20WAIT 
    MOV AL, 0XAE
    OUT 0X64, AL

    CALL .A20WAIT
    STI
    RET

.A20WAIT:
    IN AL, 0X64
    TEST AL, 2
    JNZ .A20WAIT
    RET
.A20WAIT2:
    IN AL, 0X64
    TEST AL, 1
    JZ .A20WAIT2
    RET

LOAD_GDT:
    CLI
    LEA EAX, [GDT_DESCRIPTOR]
    LGDT [EAX]

    MOV EAX, CR0
    OR EAX, 1
    MOV CR0, EAX

    JMP 0X08:PROTECTED_MODE

%INCLUDE "source/GDT.ASM"


[BITS 32]
[extern main]
[extern PRINT]
PROTECTED_MODE:
    MOV AX, 0X10
    MOV DS, AX
    MOV ES, AX
    MOV SS, AX
    MOV FS, AX
    MOV GS, AX


    MOV ESP, 0X90000
    MOV EBP, ESP

    CALL VGA_CLEARSCREEN


    PUSH DWORD TEST_TEXT
    CALL PRINT
    ADD ESP, 4

    PUSH DWORD INSTALLING_IDT
    CALL PRINT
    ADD ESP, 4
    
;     MOV EBX, 0X00010000
;     MOV EAX, 50
;     MOV ECX, 0
; .READ_LOOP:
;     PUSH EAX
;     PUSH EBX
;     CALL READ_DISK
;     ADD ESP, 8

;     INC EAX
;     INC ECX
;     ADD EBX, 512
;     CMP ECX, 50
;     JL .READ_LOOP

    CALL main

    HLT
    JMP $

section .data
TEST_TEXT: DB "Hello, World....", 0xa, 0
INSTALLING_IDT : DB "[Install] IDT...", 0xa, 0
INSTALL_IDT_DONE: DB "[DONE] INSTALLING IDT...", 0XA, 0

%INCLUDE "source/VGA.ASM"

