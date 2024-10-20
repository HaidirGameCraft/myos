%DEFINE VGA_ADDRESS 0XB8000
%DEFINE VGA_CHAR_WIDTH 80
%DEFINE VGA_CHAR_HEIGHT 25

section .data
VGA_COLUMN: DD 0
VGA_LINES: DD 0

section .text
; VGA_CLEARSCREEN - clear screen
; Return - Null
VGA_CLEARSCREEN:
    PUSH EBP
    SUB ESP, 0X6

    PUSH EAX

    ; reset VGA_COLUMN and VGA_LINES into 0
    XOR EAX, EAX
    MOV BYTE [VGA_COLUMN], AL
    MOV BYTE [VGA_LINES], AL

    POP EAX
    MOV WORD [ESP - 2], 0
    MOV DWORD [ESP - 6], VGA_ADDRESS
.LOOP:
    MOV ECX, [ESP - 6]

    MOV AH, 0X0F
    MOV AL, ' '
    MOV WORD [ECX], AX

    ADD ECX, 2
    MOV DWORD [ESP - 6], ECX
    
    MOV EAX, [ESP - 2]
    INC EAX
    MOV WORD [ESP - 2], AX 

    CMP WORD [ESP - 2], 80 * 25
    JL .LOOP

    ADD ESP, 0X6
    POP EBP
    RET

; PUTCHAR - put text into screen, position will be increase
; Arguments: 
;   -   char text - 0x08
; Return - Null
GLOBAL PUTCHAR
PUTCHAR:
    PUSH EBP
    MOV EBP, ESP

    MOV ESI, VGA_ADDRESS

    MOV AL, BYTE [EBP + 8]
    CMP AL, 0X00
    JE .END

    CMP AL, 10
    JNE .NEXT

    MOV DWORD [VGA_COLUMN], 0
    ADD DWORD [VGA_LINES], 1

    JMP .END

.NEXT:
    ; ESI = VGA_LINES * VGA_CHAR_WIDTH + VGA_COLUMN + ESI
    MOV EAX, DWORD [VGA_LINES]
    MOV ECX, VGA_CHAR_WIDTH
    MUL ECX
    ADD EAX, DWORD [VGA_COLUMN]
    MOV ECX, 2
    MUL ECX
    ADD ESI, EAX

    MOV AH, 0x0F
    MOV AL, BYTE [EBP + 8]
    MOV WORD [ESI], AX

    MOV ECX, DWORD [VGA_COLUMN]
    INC ECX
    MOV DWORD [VGA_COLUMN], ECX
.END:    
    MOV ESP, EBP
    POP EBP
    RET