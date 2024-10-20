#include "port.h"

uint8_t inb(uint16_t port) {
    uint8_t r;
    asm volatile("in %%dx, %%al" : "=a"(r) : "d"(port));
    return r;
}
uint16_t inw(uint16_t port) {
    uint16_t r;
    asm volatile("in %%dx, %%ax" : "=a"(r) : "d"(port));
    return r;
}

void outb(uint16_t port, uint8_t data) {
    asm volatile ( "out %%al, %%dx" : : "a"(data), "d"(port));
}
void outw(uint16_t port, uint16_t data) {
    asm volatile ( "out %%ax, %%dx" : : "a"(data), "d"(port));
}