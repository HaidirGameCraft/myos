#include "vga.h"

#define _VGA_CHAR_ADDRESS_ 0xB8000
#define WIDTH_CHAR 80
#define HEIGHT_CHAR 25

int vga_column = 0;
int vga_line = 0;

uint16_t* vga_chars = (uint16_t*) _VGA_CHAR_ADDRESS_;
uint16_t vga_color = 0x0F00;

void VGA_ClearScreen() {
    //VGA_DisableCursor();
    vga_column = 0;
    vga_line = 0;
    vga_color = 0x0F00;

    for(int i = 0; i < WIDTH_CHAR * HEIGHT_CHAR; i++) {
        vga_chars[i] = vga_color | ' ';
    }

}

void VGA_ScrollUp() {
    for(int y = 1; y < HEIGHT_CHAR; y++) {
        for(int x = 0; x < WIDTH_CHAR; x++) {
            vga_chars[ x + (y - 1) * WIDTH_CHAR ] = vga_chars[ x + ( y ) * WIDTH_CHAR];
        }
    }

    // Clearing Bottom Input
    for(int x = 0; x < WIDTH_CHAR; x++) {
        vga_chars[x + 24 * WIDTH_CHAR] = 0x0000;
    }
}

void PutChar(char c) {
    int index = vga_column + vga_line * WIDTH_CHAR;

    // Scrolling Up

    if( c == '\n' )
    {
        vga_column = 0;
        vga_line++;
        goto end_print_char;
    } else if ( c == 0 ) {
        return;
    } else if ( c == 0x08 ) {
        vga_column--;
        if( vga_column == 0 && vga_line > 0) {
            vga_column = WIDTH_CHAR;
            vga_line--;
        }

        vga_chars[vga_column + vga_line * WIDTH_CHAR] = 0x0000;
        goto end_print_char;
    }

    vga_chars[vga_column + vga_line * WIDTH_CHAR] = vga_color | c;
    vga_column++; 

// Updating VGA_COLUMN and VGA_LINE
end_print_char:
    index = vga_column + vga_line * WIDTH_CHAR;
    vga_column = index % WIDTH_CHAR;
    vga_line = (int)( index / WIDTH_CHAR);

    // VGA_SetCursorPosition( vga_column, vga_line);

    if( vga_line >= HEIGHT_CHAR )
    {
        VGA_ScrollUp();
        vga_line--;
    }
}

// void VGA_SetCursorPosition(int x, int y) {
//     uint16_t pos = x + y * WIDTH_CHAR;

//     outb(0x3D4, 0x0F);
// 	outb(0x3D5, (uint8_t) (pos & 0xFF));
// 	outb(0x3D4, 0x0E);
// 	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
// }
// void VGA_EnableCursor( uint8_t cursor_start, uint8_t cursor_end ) {
//     outb(0x03D4, 0x0A);
//     outb(0x03D5, ( inb(0x03D5) & 0xC0) | cursor_start);
    
//     outb(0x03D4, 0x0B);
//     outb(0x03D5, ( inb(0x03D5) & 0xE0) | cursor_end);
// }
// void VGA_DisableCursor() {
//     outb(0x03d4, 0x0A);
//     outb(0x03d5, 0x20);
// }