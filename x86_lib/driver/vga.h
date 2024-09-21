#ifndef VGA_HEADER
#define VGA_HEADER

// #define __VGA_TEXT_MODE__
#define __VGA_VIDEO_MODE__

#include <stdint.h>

void VGA_ClearScreen(uint8_t color);
void VGA_PutPixel(int x, int y, uint8_t color);
void VGA_FillRect(int x, int y, int width, int height, uint8_t color);
void VGA_DrawLine(int x0, int y0, int x1, int y1, uint8_t color);
void VGA_FillCircle(int x, int y, int radius, uint8_t color);

void VGA_ScrollUp();
void Text_Reset();
void PutChar(char c);

void VGA_SetCursorPosition(int x, int y);
void VGA_EnableCursor( uint8_t x, uint8_t y );
void VGA_DisableCursor();

#endif