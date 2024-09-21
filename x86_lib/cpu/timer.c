#include "isr.h"
#include <timer.h>
#include <driver/port.h>
#include <stdio.h>

#define FREQUENCY 1193182
#define TIMER_IO_COMMAND 0x43
#define TIMER_CHANNEL_0 0x40
#define TIMER_CHANNEL_1 0x41
#define TIMER_CHANNEL_2 0x42

int tick = 0;
int tick_sleep = 0;

void Timer_Handler(register_t _register_);

void Timer_Initialize( uint32_t frequency_value ) {
    Register_Interrupt(32, Timer_Handler);

    int freq = FREQUENCY / frequency_value;

    outb(TIMER_IO_COMMAND, 0b00110110);
    outb(TIMER_CHANNEL_0, freq & 0xFF);
    outb(TIMER_CHANNEL_0, (freq >> 8) & 0xFF);
}

void Timer_Handler(register_t _register_) {
    tick++;
    //print("Timer.");
    UNUSED(_register_);
}

void sleep( int miliseconds ) {
    tick_sleep = tick + miliseconds;

    while( tick < tick_sleep ) {
        __asm__ volatile ("hlt");
    }
}