#include <stdint.h>

extern void CLEAR_SCREEN(uint32_t color);
extern void PRINT_C(const char* text);
extern void PUTC(uint8_t c);

extern char* sys_read_file(const char* name);
extern void sys_close_file(void* file);

const char *text = "Hello, World";
int main() {
    //PUTC(text[2]);
    CLEAR_SCREEN(0xFF0000FF);
    // for(int i = 0; i < 12; i++) {
    //     PUTC(text[i]);
    //}
    PRINT_C("Welcome to another program section...\n");
    char* buffer = sys_read_file("file1.txt");
    PRINT_C(buffer);
    sys_close_file(buffer);

    return 0xDEAD2024;
}
