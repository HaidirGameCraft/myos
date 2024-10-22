#include "keyboard.h"
#include <cpu/isr.h>
#include <kmem.h>
#include <driver/vga.h>
#include <stdio.h>
#include <string.h>
#include "port.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_COMMAND_PORT 0x64

uint32_t* keyboard_interrupt;
uint32_t keyboard_ptr;
int keyboard_interrupt_index = 0;
uint8_t is_shifted = FALSE;

char scancode_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0x8, 
    0xb, 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0xa,
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+',
    '1', '2', '3', '0', 0, 0, 0, 0, 0, 0
};

char scancode_ascii_shifted[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0x8, 
    0xb, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0xa,
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+',
    '1', '2', '3', '0', 0, 0, 0, 0, 0, 0
};

void Keyboard_Handler(register_t _register_);

void Keyboard_Initialize() {
    Register_Interrupt(33, Keyboard_Handler);
    keyboard_interrupt = (uint32_t*) kmalloc( sizeof( uint32_t ) * 128 );
    keyboard_interrupt_index = 0;
    memempty( keyboard_interrupt, sizeof( uint32_t ) * 128 );
}

void Keyboard_Handler(register_t _register_) {
    uint8_t scancode = inb(KEYBOARD_DATA_PORT);
    uint8_t button = 0;

    // When key is released
    if( scancode > 0x80 ) {
        scancode = scancode - 0x80;
        button = 2;
    }
    else // When key is pressed
        button = 1;

    if ( button == KEYBOARD_BUTTON_PRESS ) {
        if( scancode == SCANCODE_LEFT_SHIFT ) {
            is_shifted = TRUE;
        }
    } else if ( button == KEYBOARD_BUTTON_RELEASE ) {
        if( scancode == SCANCODE_LEFT_SHIFT ) {
            is_shifted = FALSE;
        }
    }

    for(int i = 0; i < 128; i++) {
        if( keyboard_interrupt[i] < 0x00100000 ) continue;

        char _char_ = ( is_shifted == TRUE ) ? scancode_ascii_shifted[ (int) scancode] : scancode_ascii[ (int) scancode ];
        KeyboardCallBack callback = (KeyboardCallBack) keyboard_interrupt[i];
        callback(scancode, _char_, button);
    }

}

void AddKeyboardHandler(KeyboardCallBack handler) {
    keyboard_interrupt[keyboard_interrupt_index] = (uint32_t) handler;
    keyboard_interrupt_index++;
    
    // keyboard_ptr = handler;
    //keyboard_interrupt[keyboard_interrupt_index++] = handler;

}