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
#include <driver/keyboard.h>

void user_keyboard(uint8_t scancode, char key, uint8_t button);
char *user_input_buffer;
int ibuffer = 0;
char VOLUME_NAME[11];

extern void intV86();

void kernel_main(VBE_Mode_Info* vbe_info) {

    Initialize_VBE_Info( vbe_info );
    ClearScreen(0x00000000);

    //VGA_ClearScreen(0x00);

    print("[ Install ] Initialize GDT ");
    Initialize_GDT();
    println(" [ DONE ] ");

    print("[ Install ] Installing ISR And IRQ...");
    Install_ISR_And_IRQ();  
    println(" [ DONE ] ");

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

    ClearScreen(0x00000000);
    Text_Reset();

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

    print(VOLUME_NAME);
    print(":>");

    while ( *sys_exit != TRUE );

    return;
}

void user_keyboard(uint8_t scancode, char key, uint8_t button) {
    if( button == KEYBOARD_BUTTON_PRESS ) {
        if( scancode == SCANCODE_ENTER ) {
            putc('\n');
            CMD_Command( user_input_buffer );
            memset(user_input_buffer, 0, 1024);
            ibuffer = 0;
            print(VOLUME_NAME);
            print(":>");
        }
        else if ( scancode == SCANCODE_BACKSPACE ) {
            if( ibuffer <= 0 ) return;
            ibuffer--;
            putc( 0x8 );
            user_input_buffer[ibuffer] = key;
        } else {
            if ( scancode == SCANCODE_LEFT_SHIFT ) return;
            putc(key);
            user_input_buffer[ibuffer++] = key;
        }
    }
}