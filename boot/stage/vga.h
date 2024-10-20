#ifndef VGA_HEADER
#define VGA_HEADER

#include "stdint.h"

void VGA_ClearScreen();
void VGA_ScrollUp();
void PutChar(char c);

void VGA_SetCursorPosition(int x, int y);
void VGA_EnableCursor( uint8_t x, uint8_t y );
void VGA_DisableCursor();

#endif