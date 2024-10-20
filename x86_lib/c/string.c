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

void strtrim( char* str ) {
    int length = strlen( str );
    for(int i = 0; i < length; i++) {
        if( str[i] >= 32 && str[i] < 127 ) continue;

        for(int j = i; j < length; j++) {
            if( j < length && str[j] >= 32 && str[j] < 127) {
                str[i] = str[j];
                str[j] = 0;
                break;
            }
        }
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

    if( !value ) {
        buffer[i++] = '0';
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
char* htos(int value) {
    char* buffer = (char*) malloc( sizeof(uint32_t) * 2 + 1);
    memset(buffer, 0, sizeof(uint32_t) * 2 + 1);
    memset(buffer, '0', sizeof(uint32_t) * 2);
    int tmp = value, i = 0;
    
    while( tmp > 0 ) {
        buffer[i++] = words_hex[(tmp % 16) & 0xF];
        tmp >>= 4;
    }

    strreverse( buffer );
    return buffer;
}

char* uhtos(uint32_t value) {
    char* buffer = (char*) malloc( sizeof(uint32_t) * 2 + 1);
    memset(buffer, 0, sizeof(uint32_t) * 2 + 1);
    memset(buffer, '0', sizeof(uint32_t) * 2);
    uint32_t tmp = value, i = 0;
    
    while( tmp > 0 ) {
        buffer[i++] = words_hex[(tmp % 16) & 0xF];
        tmp >>= 4;
    }

    strreverse( buffer );
    return buffer;
}

void s_uhtos(uint32_t value, void* buffer) {
    uint32_t tmp = value, i = 0;
    
    while( tmp > 0 ) {
        ((uint8_t*) buffer)[i++] = words_hex[(tmp % 16) & 0xF];
        tmp >>= 4;
    }

    ((char*) buffer)[i] = '\0';
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

char* strlowercase(char* str){
    int size = strlen( str );
    for(int i = 0; i < size; i++) {
        str[i] = tolowercase( str[i] );
    }
    return str;
}
char* struppercase(char* str){
    int size = strlen( str );
    for(int i = 0; i < size; i++) {
        str[i] = touppercase( str[i] );
    }
    return str;
}

char* sstrlowercase(char* str, int size) {
    for(int i = 0; i < size; i++) {
        str[i] = tolowercase( str[i] );
    }
    return str;
}
char* sstruppercase(char* str, int size) {
    for(int i = 0; i < size; i++) {
        str[i] = touppercase( str[i] );
    }
    return str;
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
