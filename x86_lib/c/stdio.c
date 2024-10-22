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

char hex_word[] = "0123456789abcdef";
char is_char_hex(char h) {
    int c = 0;
    int size = strlen( hex_word );
    while( c < size ) {
        if( hex_word[c] == h ) return c;
        c++;
    }

    return 0xFFFF; // Undefined
}

void printf(const char* text, ...) {
    uint32_t* ptr = (uint32_t*)(&text + 1);
    int count = 0;
    while( text[count] != 0 ) {

        if( text[count] == '%' ) {
            if( text[count + 1] == 'i' ) {
                char* val = itos( *(ptr++) );
                print( val );
                kfree( val );
                count += 2;
            } else if ( text[count + 1] == 'x' ) {
                char* val = uhtos( *(ptr++) );
                print( val );
                kfree( val );
                count += 2;
            } else if ( text[count + 1] == 's' ) {
                print( *(ptr++) );
                count += 2;
            } else {
                PutChar(text[count]);
                count++;    
            }
        } else if ( text[count] == '\\' ) {
            count++;
            if( strncmp(text + count, "color:", 6) == 0) {
                count += 6;
                
                short red = is_char_hex(text[count++]);
                short green = is_char_hex(text[count++]);
                short blue = is_char_hex(text[count++]);

                if( red != -1 && green != -1 && blue != -1 ) {
                    red = (short)( (float)red / 16 * 255 );
                    green = (short)( (float)green / 16 * 255);
                    blue = (short)( (float)blue / 16 * 255);
                    SetTextColor(( blue << 16 ) | ( green << 8 ) | red);
                }
            }
        }
        else {
            PutChar(text[count]);
            count++;
        }

    }

    SetTextColorDef();
}

void println(const char* text) {
    print(text);
    putc('\n');
}

void printh( int value ) {
    
}
