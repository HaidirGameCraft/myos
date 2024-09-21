[ORG 0X7C00]
[BITS 16]

JMP SHORT BOOT_START
NOP

OEM: db "mkfs.fat"
Bytes_Per_Sector: dw 512
Sectors_Per_Cluster: db 4
Reserved_Sectors: dw 4
FAT_Count: db 2
Root_Dir_Entries: dw 512
Total_Sectors: dw 0
Media_Type: db 0xf8
Sectors_Per_FAT: dw 0x64
Sector_Per_Track: dw 0x20
Number_Of_Heads: dw 0x08
Hidden_Sector: dd 0x0
Large_Sector: dd 0x00018d60


Drive_Number: db 0x80
            db 0
Boot_Signature: db 0x29
Serial_Volume: db 0x49, 0x46, 0xf6, 0x47
Volume_Name: db "LOCAL DRIVE"
FATType: db 'FAT16   '

BOOT_START:
    MOV AX, 0
    MOV DS, AX
    MOV ES, AX
    MOV SS, AX
    MOV FS, AX
    MOV GS, AX

    MOV SP, 0X7C00
    MOV BP, SP

    MOV [Drive_Number], DL

    MOV SI, TEXT_MESSAGE_BIOS
    CALL PRINT

    MOV AX, 2
    MOV CX, 30
    MOV BX, 0X1000
    CALL READ_SECTOR

    ; mov ax, 0x4f02
    ; mov cx, 0x203
    ; int 0x10

    ; mov ax, 0x0012
    ; int 10h

    JMP 0X1000

    HLT
    JMP $

; READ SECTOR - TO READ SECTOR FROM DISK TO LOAD THE SPECIFIC ADDRESS
; AX - LBA ADDRESS
; CX - COUNTER READ (HOW MUCH DID YOU WANT TO READ?)
; BX - BUFFER ADDRESS / WHAT ADDRESS TO LOAD SECTOR
READ_SECTOR:
    MOV DI, 5
.MAIN:
    PUSH AX
    PUSH BX
    PUSH CX
    PUSH DX

    PUSHA
    XOR DX, DX
    MOV CX, [Sector_Per_Track]
    DIV CX

    INC DL
    MOV BYTE [ABS_SECTOR], DL

    XOR DX, DX
    MOV CX, [Number_Of_Heads]
    DIV CX

    MOV BYTE [ABS_HEADS], DL
    MOV BYTE [ABS_CYLINDER], AL

    POPA

    MOV AH, 0X02
    MOV AL, 1
    MOV DL, [Drive_Number]
    MOV CH, [ABS_CYLINDER]
    MOV CL, [ABS_SECTOR]
    MOV DH, [ABS_HEADS]
.READ_LOOP:

    INT 0X13
    JNC .READ_DONE

    MOV AH, 0X00
    MOV AL, 0X00
    INT 0X13

    POP DX
    POP CX
    POP BX
    POP AX

    DEC DI
    TEST DI, DI
    JNZ .MAIN

    MOV SI, ERROR_LOAD_DISK
    CALL PRINT

    HLT
    JMP $
.READ_DONE:
    POP DX
    POP CX
    POP BX
    POP AX

    INC AX
    ADD BX, 512
    DEC CX
    TEST CX, CX
    JNZ .MAIN

    RET

%INCLUDE "source/PRINT.ASM"
    

cluster: dw 0
ABS_SECTOR: DB 0
ABS_HEADS: DB 0
ABS_CYLINDER DB 0
OS_FILE: db "OS      SYS"

TEXT_MESSAGE_BIOS: DB "OS DOS FROM HAIDIR...", 0XD, 0XA, 0X0
test: DB "TEST...", 0xd, 0xa, 0x0
ERROR_LOAD_DISK: DB "ERROR LOAD DISK...", 0XD, 0XA, 0X0

TIMES 510-($-$$) DB 0
DW 0XAA55
buffer: