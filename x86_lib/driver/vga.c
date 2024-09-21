#include "vga.h"
#include <cmath.h>
#include <vesa.h>
#include <font.h>

#define _VGA_CHAR_ADDRESS_ 0xB8000
#define _VGA_VIDEO_ADDRESS_ 0xA0000
#define WIDTH_CHAR 100
#define HEIGHT_CHAR 37

#define WIDTH_SCREEN 640
#define HEIGHT_SCREEN 480

int vga_column = 0;
int vga_line = 0;

uint16_t* vga_chars = (uint16_t*) _VGA_CHAR_ADDRESS_;
uint8_t* vga_video = (uint8_t*) _VGA_VIDEO_ADDRESS_;
uint16_t vga_color = 0x0F00;

void VGA_ClearScreen(uint8_t color) {
#if defined(__VGA_TEXT_MODE__)
    VGA_DisableCursor();
    vga_column = 0;
    vga_line = 0;
    vga_color = 0x0F00;

    for(int i = 0; i < WIDTH_CHAR * HEIGHT_CHAR; i++) {
        vga_chars[i] = vga_color | ' ';
    }

    VGA_EnableCursor(0, 16);
    VGA_SetCursorPosition(vga_column, vga_line);
#elif defined(__VGA_VIDEO_MODE__)
    vga_column = 0;
    vga_line = 0;
    for(int y = 0; y < HEIGHT_SCREEN; y++) {
        for(int x = 0; x < WIDTH_SCREEN; x++) {
            vga_video[ x + y * WIDTH_SCREEN ] = color;
        }
    }
#endif
}

void Select_BitPlane(uint8_t plane) {
    outb( 0x03C4, 0x02);
    outb( 0x03C5, 1 << plane);
}

void VGA_PutPixel(int x, int y, uint8_t color) {
    int index = (x + y * WIDTH_SCREEN) / 8;

    for(uint8_t p = 0; p < 4; p++) { 
        uint8_t mask = 1 << (7 - (x % 8));
        if( color & (1 << p) ) {
            vga_video[ index ] |= mask;
        } else {
            vga_video[ index ] &= ~mask;
        }
    }
}

void VGA_FillRect(int x, int y, int width, int height, uint8_t color) {
    int index = x + y * WIDTH_SCREEN;

    for(int i = y; i < HEIGHT_SCREEN; i++) {

        if( i >= y + height) break; // Offscreen Y

        for(int j = x; j < WIDTH_SCREEN; j++) {
            if( j >= x + width ) break; // Offscreen X

            VGA_PutPixel( j, i, color );
        }
    }
}

// DDA Line Generation Algorithm (from: geeksforgeeks.org)
void VGA_DrawLine(int x0, int y0, int x1, int y1, uint8_t color) {
    int dx = x1 -x0;
    int dy = y1 - y0;

    int steps = abs( dx ) > abs( dy ) ? abs( dx ) : abs( dy );

    float x_inc = dx / (float) steps;
    float y_inc = dy / (float) steps;

    float x = x0;
    float y = y0;
    for(int i = 0; i < steps; i++) {
        VGA_PutPixel( (int)(x), (int)(y), color );
        x += x_inc;
        y += y_inc;
    }
}

void VGA_FillCircle(int x, int y, int radius, uint8_t color) {

    int X1 = x - radius;
    int Y1 = y - radius;
    int X2 = x + radius;
    int Y2 = y + radius;


    X1 = (X1 < 0) ? 0 : X1;
    X1 = (X1 >= WIDTH_SCREEN) ? WIDTH_SCREEN : X1;
    X2 = (X2 < 0) ? 0 : X2;
    X2 = (X2 >= WIDTH_SCREEN) ? WIDTH_SCREEN : X2;

    Y1 = (Y1 < 0) ? 0 : Y1;
    Y1 = (Y1 >= HEIGHT_SCREEN) ? HEIGHT_SCREEN : Y1;
    Y2 = (Y2 < 0) ? 0 : Y2;
    Y2 = (Y2 >= HEIGHT_SCREEN) ? HEIGHT_SCREEN : Y2;

    for(int i = Y1; i < Y2; i++) {
        for(int j = X1; j < X2; j++) {
            float dist = sqrt((j - x)*(j - x) + (i - y)*(i - y));
            if( dist <= (float) radius) {
                VGA_PutPixel(j, i, color);
            }
        }
    }
}














void Text_Reset() {
    vga_column = 0;
    vga_line = 0;
}


void VGA_ScrollUp() {
#if defined(__VGA_TEXT_MODE__)
    for(int y = 1; y < HEIGHT_CHAR; y++) {
        for(int x = 0; x < WIDTH_CHAR; x++) {
            vga_chars[ x + (y - 1) * WIDTH_CHAR ] = vga_chars[ x + ( y ) * WIDTH_CHAR];
        }
    }

    // Clearing Bottom Input
    for(int x = 0; x < WIDTH_CHAR; x++) {
        vga_chars[x + 24 * WIDTH_CHAR] = 0x0000;
    }
#elif defined(__VGA_VIDEO_MODE__)

    for(int y = 0; y < vbe_ptr->height - 16; y++) {
        for(int x = 0; x < vbe_ptr->width; x++) {
            int index = (x + y * vbe_ptr->width);
            int target = (x + (y + 16) * vbe_ptr->width);

            uint32_t color = GetPixel( x, y + 16 );
            PutPixel( x, y, color );
        }
    }

    for(int y = vbe_ptr->height - 16; y < vbe_ptr->height; y++) {
        for(int x = 0; x < vbe_ptr->width; x++) {
            PutPixel( x, y, 0x00000000 );
        }
    }

#endif
}

void PutChar(char c) {
int index = vga_column + vga_line * WIDTH_CHAR;
#if defined(__VGA_TEXT_MODE__)

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

    VGA_SetCursorPosition( vga_column, vga_line);

    if( vga_line >= HEIGHT_CHAR )
    {
        VGA_ScrollUp();
        vga_line--;
    }
#elif defined(__VGA_VIDEO_MODE__)

    if( c == '\n' )
    {
        vga_column = 0;
        vga_line++;
        goto end_print_char;
        return;
    } else if ( c == 0 ) {
        return;
    } else if ( c == 0x08 ) {
        vga_column--;
        if( vga_column == 0 && vga_line > 0) {
            vga_column = WIDTH_CHAR;
            vga_line--;
        }

        uint8_t* bitmap = Font_GetChar(' ');
        for(int y = 0; y < 16; y++) {
            int local_y = vga_line * 16 + y;
            for(int x = 0; x < 8; x++) {
                int local_x = vga_column * 8 + x;

                uint8_t l = (bitmap[y] >> (7 - x)) & 0x1;
                if( l == 1 ) {
                    PutPixel(local_x, local_y, 0x00FFFFFF);
                } else {
                    PutPixel(local_x, local_y, 0x00000000);
                }
            }
        }
        goto end_print_char;
        return;
    }

    uint8_t* bitmap = Font_GetChar(c);
    for(int y = 0; y < 16; y++) {
        int local_y = vga_line * 16 + y;
        for(int x = 0; x < 8; x++) {
            int local_x = vga_column * 8 + x;

            uint8_t l = (bitmap[y] >> (7 - x)) & 0x1;
            if( l == 1 ) {
                PutPixel(local_x, local_y, 0x00FFFFFF);
            }
        }
    }

    vga_column++;
end_print_char:
    index = vga_column + vga_line * WIDTH_CHAR;
    vga_column = index % WIDTH_CHAR;
    vga_line = (int)( index / WIDTH_CHAR);

    if( vga_line >= HEIGHT_CHAR )
    {
        VGA_ScrollUp();
        vga_line--;
    }
#endif
}

void VGA_SetCursorPosition(int x, int y) {
    uint16_t pos = x + y * WIDTH_CHAR;

    outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
void VGA_EnableCursor( uint8_t cursor_start, uint8_t cursor_end ) {
    outb(0x03D4, 0x0A);
    outb(0x03D5, ( inb(0x03D5) & 0xC0) | cursor_start);
    
    outb(0x03D4, 0x0B);
    outb(0x03D5, ( inb(0x03D5) & 0xE0) | cursor_end);
}
void VGA_DisableCursor() {
    outb(0x03d4, 0x0A);
    outb(0x03d5, 0x20);
}