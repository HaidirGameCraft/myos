#ifndef __STRING_HEADER__
#define __STRING_HEADER__

#include <stdint.h>

void memset(void* _dest, uint8_t _value, uint32_t size);
void memcpy(void* _dest, void* _src, uint32_t size);
void memempty(void* _dest, uint32_t size);

int strlen(char* str);
uint8_t strcmp(char* _cmp1_, char* _cmp2_);
uint8_t strncmp(char* _cmp1_, char* _cmp2_, int size);
char* itos(int value);
void htos(int value, char* buffer);
void uhtos(uint32_t value, char* buffer);
int soth(char*buffer);
uint8_t soti(char*buffer, int *value);
void strreverse(char* buffer);
void strappend( char* buffer, char val );
char touppercase(char c);
char tolowercase(char c);
char* strsplit(const char* text, char _reg_);
void strsplit_free( char* array );

#endif