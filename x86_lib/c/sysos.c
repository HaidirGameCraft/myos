#include <cpu/isr.h>
#include <vesa.h>
#include <stdio.h>
#include <fat.h>


void syscall_handler(register_t reg) {
    printf("Interrupt syscall 0x%x\n", reg.eax);
    switch ( reg.eax )
    {
    case 0x00:
        while( TRUE )
            __asm__ ("hlt");
        break;
    case 0x01: // Read file
        reg.eax = (uint32_t) FAT_LoadFile((char*)reg.esi);
        break;
    case 0x04: // Free FAT File
        FAT_FreeFile((void*) reg.ebx);
        break;
    case 0x10: // Clear Screen with color
        ClearScreen( reg.ebx );
        break;
    case 0x11: // Put Pixel to the address
        PutPixel(reg.ebx, reg.ecx, reg.edx);
        break;
    case 0x20: // Insert Text
        print( (char*) reg.esi );
        break;
    case 0x21: // Put Character
        putc( reg.edx );
        break;
    case 0x30: // Malloc
        reg.eax = (uint32_t) malloc(reg.ebx);
        break;
    case 0x31: // Free Malloc
        free( reg.ebx );
        break;        
    default:
        break;
    }
}
