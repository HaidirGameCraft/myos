#include "isr.h"
#include "idt.h"
#include <timer.h>
#include <stdio.h>
#include <driver/keyboard.h>

uint32_t interrupt_handler[256];

void remapPIC();

void Install_ISR_And_IRQ() {
    Set_IDT_Gate( 0, (uint32_t) &ISR0, 0x8E);
    Set_IDT_Gate( 1, (uint32_t) &ISR1, 0x8E);
    Set_IDT_Gate( 2, (uint32_t) &ISR2, 0x8E);
    Set_IDT_Gate( 3, (uint32_t) &ISR3, 0x8E);
    Set_IDT_Gate( 4, (uint32_t) &ISR4, 0x8E);
    Set_IDT_Gate( 5, (uint32_t) &ISR5, 0x8E);
    Set_IDT_Gate( 6, (uint32_t) &ISR6, 0x8E);
    Set_IDT_Gate( 7, (uint32_t) &ISR7, 0x8E);
    Set_IDT_Gate( 8, (uint32_t) &ISR8, 0x8E);
    Set_IDT_Gate( 9, (uint32_t) &ISR9, 0x8E);
    Set_IDT_Gate(10, (uint32_t) &ISR10, 0x8E);
    Set_IDT_Gate(11, (uint32_t) &ISR11, 0x8E);
    Set_IDT_Gate(12, (uint32_t) &ISR12, 0x8E);
    Set_IDT_Gate(13, (uint32_t) &ISR13, 0x8E);
    Set_IDT_Gate(14, (uint32_t) &ISR14, 0x8E);
    Set_IDT_Gate(15, (uint32_t) &ISR15, 0x8E);
    Set_IDT_Gate(16, (uint32_t) &ISR16, 0x8E);
    Set_IDT_Gate(17, (uint32_t) &ISR17, 0x8E);
    Set_IDT_Gate(18, (uint32_t) &ISR18, 0x8E);
    Set_IDT_Gate(19, (uint32_t) &ISR19, 0x8E);
    Set_IDT_Gate(20, (uint32_t) &ISR20, 0x8E);
    Set_IDT_Gate(21, (uint32_t) &ISR21, 0x8E);
    Set_IDT_Gate(22, (uint32_t) &ISR22, 0x8E);
    Set_IDT_Gate(23, (uint32_t) &ISR23, 0x8E);
    Set_IDT_Gate(24, (uint32_t) &ISR24, 0x8E);
    Set_IDT_Gate(25, (uint32_t) &ISR25, 0x8E);
    Set_IDT_Gate(26, (uint32_t) &ISR26, 0x8E);
    Set_IDT_Gate(27, (uint32_t) &ISR27, 0x8E);
    Set_IDT_Gate(28, (uint32_t) &ISR28, 0x8E);
    Set_IDT_Gate(29, (uint32_t) &ISR29, 0x8E);
    Set_IDT_Gate(30, (uint32_t) &ISR30, 0x8E);
    Set_IDT_Gate(31, (uint32_t) &ISR31, 0x8E);

    remapPIC();

    Set_IDT_Gate(32, (uint32_t) &IRQ32, 0x8E);
    Set_IDT_Gate(33, (uint32_t) &IRQ33, 0x8E);
    Set_IDT_Gate(34, (uint32_t) &IRQ34, 0x8E);
    Set_IDT_Gate(35, (uint32_t) &IRQ35, 0x8E);
    Set_IDT_Gate(36, (uint32_t) &IRQ36, 0x8E);
    Set_IDT_Gate(37, (uint32_t) &IRQ37, 0x8E);
    Set_IDT_Gate(38, (uint32_t) &IRQ38, 0x8E);
    Set_IDT_Gate(39, (uint32_t) &IRQ39, 0x8E);
    Set_IDT_Gate(40, (uint32_t) &IRQ40, 0x8E);
    Set_IDT_Gate(41, (uint32_t) &IRQ41, 0x8E);
    Set_IDT_Gate(42, (uint32_t) &IRQ42, 0x8E);
    Set_IDT_Gate(43, (uint32_t) &IRQ43, 0x8E);
    Set_IDT_Gate(44, (uint32_t) &IRQ44, 0x8E);
    Set_IDT_Gate(45, (uint32_t) &IRQ45, 0x8E);
    Set_IDT_Gate(46, (uint32_t) &IRQ46, 0x8E);
    Set_IDT_Gate(47, (uint32_t) &IRQ47, 0x8E);

    Initialize_IDT();
}

void remapPIC() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);
}

char* exception_message[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void Install_Interrupt() {
    __asm__ volatile ("sti");

    Timer_Initialize(110); // Default Timer Frequency 50
    Keyboard_Initialize();
}

void Register_Interrupt(uint8_t number, isr_t handler) {
    interrupt_handler[ number ] = (uint32_t*) handler;
}

void ISR_Handler(register_t reg) {
    print("Interrupt Code: ");
    print(exception_message[reg.int_no]);
    print("\n");
    return;
}

void IRQ_Handler(register_t reg) {
    if( reg.int_no >= 40 ) outb(0xa0, 0x20);
    outb(0x20, 0x20);

    if( interrupt_handler[reg.int_no] != NULL) {
        isr_t handler = (isr_t) interrupt_handler[reg.int_no];
        handler( reg );
    }
}
