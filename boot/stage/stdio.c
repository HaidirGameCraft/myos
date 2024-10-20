#include "stdio.h"
#include "vga.h"

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

void println(const char* text) {
    print(text);
    putc('\n');
}

void printh( int value ) {
    
}