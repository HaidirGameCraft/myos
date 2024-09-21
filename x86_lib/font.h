#ifndef __FONT__
#define __FONT__

#include <stdint.h>

typedef struct {
    uint8_t pixel[16];
} __attribute__((packed)) FontChar;

FontChar* ascii_font;
void Initialize_Font();
uint8_t* Font_GetChar( uint8_t c );

#endif