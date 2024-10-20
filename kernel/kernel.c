#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <timer.h>
#include <cmd.h>
#include <fat.h>
#include <memory.h>
#include <cpu/isr.h>
#include <driver/vga.h>
#include <cpu/gdt.h>
#include <page.h>
#include <vesa.h>
#include <elf.h>
#include <driver/keyboard.h>

void user_keyboard(uint8_t scancode, char key, uint8_t button);
char *user_input_buffer;
int ibuffer = 0;
char VOLUME_NAME[11];

extern void intV86();
extern void _heap_start();
extern void _heap_end();

typedef void (*main_func)();

void* load_binary_file(uint32_t physical_address, uint8_t* binary_buffer, uint32_t size);

void kernel_main(VBE_Mode_Info* vbe_info) {

    Initialize_VBE_Info( vbe_info );
    ClearScreen(0x00000000);

    print("[ Install ] Initialize GDT ");
    Initialize_GDT();
    println(" [ DONE ] ");

    print("[ Install ] Installing ISR And IRQ...");
    Install_ISR_And_IRQ();  
    println(" [ DONE ] ");

//    Initialize_Paging();

    print("[ Initialize ] Heap...");
    initialize_heap();
    println(" [ DONE ] ");
    
    print("[ Install ] Installing Interrupt...");
    Install_Interrupt();
    println(" [ DONE ] ");

    Initialize_Paging();

    FAT_Initialize();
    CMD_Initialize();

    FileSystemInfo* info = GetInfoFileSystem();
    memcpy( VOLUME_NAME, info->volume_name, 11 );

    print("[ Keyboard ] Add Keyboard Handle\n");
    user_input_buffer = (char*) malloc( 1024 );
    memset( user_input_buffer, 0, 1024 );
    AddKeyboardHandler( user_keyboard );

//    ClearScreen(0x00000000);
//    Text_Reset();

    for(int i = 0; i < 100; i++) {
        print("=");
    }
    print("\n");
    println("               Welcome to MYOS :)              ");
    println("               Authur By : Haidir              ");
    for(int i = 0; i < 100; i++) {
        print("=");
    }
    print("\n");
    print("\n");

    printf("%s>", VOLUME_NAME);


    // uint8_t* binary = FAT_LoadFile("prog.bin");
    // void* entry = elf_load_file(binary);

    // int (*main_)() = (int (*)()) entry;
    // uint32_t f = main_();

    // printf("Done Excute the program of Code 0x%x", f);
    // elf_clear(binary);
    // FAT_FreeFile(binary);

    while ( *sys_exit != TRUE );

    return;
}

void user_keyboard(uint8_t scancode, char key, uint8_t button) {
    if( button == KEYBOARD_BUTTON_PRESS ) {
        if( scancode == SCANCODE_ENTER ) {
            putc('\n');
            CMD_Command( user_input_buffer );

            if( strncmp(user_input_buffer, "mmsize", 6) == 0 ) {
                char* s = uhtos(_heap_start);
                print("Memory Heap Start: ");
                println( s );
                free(s);

                s = uhtos(_heap_end);
                print("Memory Heap End: ");
                println( s );
                free(s);

                s = uhtos(_heap_end - _heap_start);
                print("Memory Heap Range: ");
                println( s );
                free(s);
            } else if ( strncmp(user_input_buffer, "read", 4) == 0 ) {
                FileDirectory* file = FAT_FindFile( user_input_buffer + 5 );
                printf("Address: 0x%x\n", file);
                char* r = FAT_ReadFile( file );
                println( r );
                free( r );
                free( file );
            } else if ( strncmp(user_input_buffer, "hwinfo", 6) == 0 ) {
                
                char* s = uhtos( vbe_ptr->physical_address);
                print("Framebuffer Address: ");
                println( s );
                free(s);
            } else if ( strncmp(user_input_buffer, "exec", 4) == 0 ) {
                FileDirectory* program_file = FAT_FindFile( user_input_buffer + 5 );

                char* program = FAT_ReadFile( program_file );
                //char* program = FAT_LoadFile(user_input_buffer + 5);
                void* entry = elf_load_file( program );

                int (*entry_)() = (int (*)()) entry;
                int r = entry_();

                printf("Program has ended with (0x%x)\n", r);
                elf_clear( program );
                FAT_FreeFile( program );
                free( program_file );
            }

            memset(user_input_buffer, 0, 1024);
            ibuffer = 0;
            print(VOLUME_NAME);
            print(":>");
        }
        else if ( scancode == SCANCODE_BACKSPACE ) {
            if( ibuffer <= 0 ) return;
            ibuffer--;
            Text_Backspace();
            user_input_buffer[ibuffer] = key;
        } else {
            if ( scancode == SCANCODE_LEFT_SHIFT ) return;
            putc(key);
            user_input_buffer[ibuffer++] = key;
        }
    }
}

void *load_binary_file(uint32_t physical_address, uint8_t* binary_buffer, uint32_t size) {
    memcpy((void*)physical_address, binary_buffer, size);
    return (void*) physical_address;
}
