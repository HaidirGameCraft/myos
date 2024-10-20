#ifndef __STRING_HEADER__
#define __STRING_HEADER__

#include <stdint.h>

void memset(void* _dest, uint8_t _value, uint32_t size);
void memcpy(void* _dest, void* _src, uint32_t size);
void memempty(void* _dest, uint32_t size);

int strlen(char* str);
void strtrim( char* str );
uint8_t strcmp(char* _cmp1_, char* _cmp2_);
uint8_t strncmp(char* _cmp1_, char* _cmp2_, int size);
char* itos(int value);
char* htos(int value);
char* uhtos(uint32_t value);
void s_uhtos(uint32_t value, void* buffer);
int soth(char*buffer);
uint8_t soti(char*buffer, int *value);
void strreverse(char* buffer);
void strappend( char* buffer, char val );
char* strlowercase(char* str);
char* struppercase(char* str);
char* sstrlowercase(char* str, int size);
char* sstruppercase(char* str, int size);
char touppercase(char c);
char tolowercase(char c);
char* strsplit(const char* text, char _reg_);
void strsplit_free( char* array );

#endif