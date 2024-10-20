section .text
[extern PUTCHAR]
; PRINT - print text/string to screen
; Arguments:
;   -   char* buffer - Address Text Buffer uint32_t 0X8
;   - 
; Return - Null  
GLOBAL PRINT
PRINT:
    PUSH EBP
    MOV EBP, ESP

    MOV ESI, DWORD [EBP + 8]
.LOOP:
    MOV AL, BYTE [SI]
    INC ESI

    CMP AL, 0X00
    JE .DONE

    PUSH ESI
    PUSH EAX
    CALL PUTCHAR
    ADD ESP, 4
    POP ESI

    JMP .LOOP
.DONE:
    MOV ESP, EBP
    POP EBP
    RET
