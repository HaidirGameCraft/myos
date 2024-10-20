#ifndef __FS__ 
#define __FS__ 

#include <stdint.h>
#include <fat.h>

struct __FILE {
    uint64_t address;
    uint64_t seek_;
};

typedef struct __FILE FILE;

FILE* fs_open(const char* path);
void fs_seek(long location);
void fs_read(void* dest, int size, FILE* file);
void fs_write(void* dest, int size, FILE* file);
void fs_close(FILE* file);

#endif
