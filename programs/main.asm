section .text
global CLEAR_SCREEN
CLEAR_SCREEN:
    push ebp
    mov ebp, esp

    mov eax, 0x10
    mov ebx, [ebp + 8]
    int 0x80

    mov esp, ebp
    pop ebp
    ret

global PRINT_C
PRINT_C:
    push ebp
    mov ebp, esp

    mov eax, 0x20
    mov esi, dword [ebp + 8]
    int 0x80

    mov esp, ebp
    pop ebp
    ret

global PUTC
PUTC:
    push ebp
    mov ebp, esp

    mov eax, 0x21
    mov edx, [ebp + 8]
    int 0x80

    mov esp, ebp
    pop ebp
    ret

global sys_read_file
sys_read_file:
    push ebp
    mov ebp, esp 

    mov eax, 0x01 
    mov esi, [ebp + 0x8]
    int 0x80


    mov esp, ebp 
    pop ebp
    ret

global sys_close_file
sys_close_file:
    push ebp
    mov ebp, esp

    mov eax, 0x04
    mov ebx, [ebp + 0x8]
    int 0x80

    mov esp, ebp
    pop ebp
    ret
