[BITS 32]

; READ_DISK - read disk
; Arguments
;   -   uint32_t LBA Address
;   -   uint8_t* BUFFER
; Return - Null
section.text
GLOBAL READ_DISK
READ_DISK:
    PUSH EBP
    MOV EBP, ESP

    MOV EAX, [EBP + 12]
    SHR EAX, 24
    AND EAX, 0X0F
    OR AL, 0XE0
    MOV DX, 0X1F6
    OUT DX, AL

    MOV AL, 1
    MOV DX, 0X1F2
    OUT DX, AL

    MOV EAX, [EBP + 12]
    MOV DX, 0X1F3
    OUT DX, AL

    SHR EAX, 8 
    MOV DX, 0X1F4
    OUT DX, AL 

    SHR EAX, 8
    MOV DX, 0X1F5
    OUT DX, AL

    MOV AL, 0X20
    MOV DX, 0X1F7
    OUT DX, AL 

.IOWAIT:
    MOV DX, 0X1F7
    IN AL, DX
    TEST AL, 0X80
    JNZ .IOWAIT

    MOV DX, 0X1F0
    MOV ECX, 0
    MOV ESI, [EBP + 8]
.LOAD_BUFFER:
    IN AX, DX
    MOV WORD [ESI], AX
    ADD ESI, 2

    INC ECX
    CMP ECX, 256
    JL .LOAD_BUFFER

    MOV ESP, EBP
    POP EBP
    RET