#include <cmd.h>
#include <vesa.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <driver/vga.h>

bool_t EXIT = FALSE;


void CMD_Initialize() {
    sys_exit = &EXIT;
}

void CMD_Command(char* buffer) {
    if( strncmp( buffer, "print", 5 ) == 0 ) {
        char* b = buffer + (5 + 1);
        println( b );
    } else if ( strncmp(buffer, "clear", 5) == 0) {
        ClearScreen(0x00000000);
        Text_Reset();
    } else if ( strcmp(buffer, "exit") == 0 ) {
        *sys_exit = TRUE;
    } else if ( strcmp(buffer, "credit") == 0 ) {
        // Credits Haidir
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
    }
}