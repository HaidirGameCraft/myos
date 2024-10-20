#include <stdio.h>
#include <string.h>
#include <driver/vga.h>

void putc(char c) {
    PutChar(c);
}

void print(const char* text) {
    int count = 0;
    while( text[count] != 0 ) {
        PutChar(text[count]);
        count++;
    }
}

void printf(const char* text, ...) {
    uint32_t* ptr = (uint32_t*)(&text + 1);
    int count = 0;
    while( text[count] != 0 ) {

        if( text[count] == '%' ) {
            if( text[count + 1] == 'i' ) {
                char* val = itos( *(ptr++) );
                print( val );
                free( val );
                count += 2;
            } else if ( text[count + 1] == 'x' ) {
                char* val = uhtos( *(ptr++) );
                print( val );
                free( val );
                count += 2;
            } else if ( text[count + 1] == 's' ) {
                print( *(ptr++) );
                count += 2;
            } else {
                PutChar(text[count]);
                count++;    
            }
        } else {
            PutChar(text[count]);
            count++;
        }

    }
}

void println(const char* text) {
    print(text);
    putc('\n');
}

void printh( int value ) {
    
}