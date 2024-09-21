#include <string.h>
#include <stdio.h>
#include <memory.h>
#include <driver/vga.h>

void memset(void* _dest, uint8_t _value, uint32_t size) {
    for(int i = 0; i < size; i++) {
        ((uint8_t*) _dest)[i] = _value;
    }
}
void memcpy(void* _dest, void* _src, uint32_t size) {
    for(int i = 0; i < size; i++) {
        ((uint8_t*) _dest)[i] = ((uint8_t*) _src)[i];
    }
}
void memempty(void* _dest, uint32_t size) {
    for(int i = 0; i < size; i++) {
        ((uint8_t*) _dest)[i] = 0;
    }
}


int strlen(char* str) {
    int count = 0;
    while( str[count] != 0) count++;
    return count;
}

uint8_t strcmp(char* _cmp1_, char* _cmp2_) {
    int i = 0;
    while( TRUE ) {

        if( _cmp1_[i] == 0 && _cmp2_[i] == 0) break;
        else if( _cmp1_[i] == 0 || _cmp2_[i] == 0) return 1;

        if( _cmp1_[i] != _cmp2_[i] ) return 1;

        i++;
    }

    return 0;
}

uint8_t strncmp(char* _cmp1_, char* _cmp2_, int size) {
    for(int i = 0; i < size; i++) {
        if( _cmp1_[i] != _cmp2_[i] ) return 1;
    }

    return 0;
}

uint8_t soti( char*buffer , int *value) {
    uint8_t sign;
    int index = 0;

    if( buffer[0] == '-'){ 
        sign = -1;
        index++;
    }
    else {
        sign = 1;
    }

    while( buffer[ index ] != 0 ) {
        *value *= 10;
        if( buffer[index] >= '0' && buffer[index] <= '9') return 1;

        uint8_t number = buffer[ index ];
        *value += number;
        index++;
    }

    return 0;

}

char* itos(int value) {
    int sign, tmp;
    int length = 0;
    if( (tmp = value) < 0){ 
        sign = -1; 
        length++;
    }
    else sign = 1;

    while( tmp > 0 ) {
        tmp /= 10;
        length++;
    }

    char* buffer = (char*) malloc( length );

    tmp = value;
    int i = 0;
    while( tmp > 0 ) {
        buffer[i++] = tmp % 10 + '0';
        tmp /= 10;
    }

    if( sign == -1 ) buffer[i++] = '-';
    buffer[i] = '\0';

    strreverse(buffer);

    return buffer;
}

void strappend( char* buffer, char val ) {
    int size = strlen(buffer);
    buffer[size] = val;
    buffer[size+1] = 0;
}

char words_hex[] = "0123456789ABCDEF";
void htos(int value, char* buffer) {
    int tmp = value, i = 0;
    
    while( tmp > 0 ) {
        buffer[i++] = words_hex[(tmp % 16) & 0xF];
        tmp >>= 4;
    }

    buffer[i++] = 'x';
    buffer[i++] = '0';
    buffer[i] = 0x0;

    strreverse( buffer );
}

void uhtos(uint32_t value, char* buffer) {
    uint32_t tmp = value, i = 0;
    
    while( tmp > 0 ) {
        buffer[i++] = words_hex[(tmp % 16) & 0xF];
        tmp >>= 4;
    }

    buffer[i++] = 'x';
    buffer[i++] = '0';
    buffer[i] = 0x0;

    strreverse( buffer );
}

void strreverse(char* buffer) {
    int size = strlen(buffer);
    int j = size - 1;
    for(int i = 0; i < size; i++) {

        if( i >= j ) break;

        char tmp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = tmp;
        j--;
    }
}

char touppercase(char c) {
    if( c >= 0x61 && c <= 0x7A )
    {
        char d = c - 0x61;
        return (char) 0x41 + d;
    }

    return c;
}
char tolowercase(char c) {
    if( c >= 0x41 && c <= 0x5A )
    {
        char d = c - 0x41;
        return (char) 0x61 + d;
    }

    return c;
}

char* strsplit(const char* text, char _reg_) {
    int split_length = 0;
    for(int i = 0; i < strlen( text ); i++) {
        if( text[i] == _reg_ ) {
            split_length++;
        }
    }
    
    uint32_t* array = (uint32_t*) malloc( 4 * split_length );
    int location = 0;
    split_length = 0;
    for(int i = 0; i < strlen( text ); i++) {
        if( text[i] == _reg_ ) {
            char* m = (char*) malloc( i - location );
            memset( m, 0, i - location);
            memcpy( m, text + location, i - location);
            array[split_length] = (uint32_t) m;
            location = i + 1;
            split_length++;
            continue;
        }
    }

    return array;
}

void strsplit_free( char* array ) {
    int length = size_memory( array ) / 4;

    uint32_t* ar = (uint32_t*) array;
    for(int i = 0; i < length; i++) {
        free( ar[i] );
    }

    free( array );
}