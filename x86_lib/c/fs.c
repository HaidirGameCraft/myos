#include <fs.h>
#include <kmem.h>


FILE* fs_open(const char* path) {
        
}

void fs_seek(long location);
void fs_read(void* dest, int size, FILE* file);
void fs_write(void* dest, int size, FILE* file);
void fs_close(FILE* file);
