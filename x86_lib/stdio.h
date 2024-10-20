#ifndef _STDIO_HEADER_
#define _STDIO_HEADER_

#include <stdint.h>

void putc(char c);
void print(const char* text);
void printf(const char* text, ...);
void println(const char* text);
void printh( int value );

#endif