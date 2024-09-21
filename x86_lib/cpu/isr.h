#ifndef _ISR_HEADER_
#define _ISR_HEADER_

#include <stdint.h>

extern void ISR0();
extern void ISR1();
extern void ISR2();
extern void ISR3();
extern void ISR4();
extern void ISR5();
extern void ISR6();
extern void ISR7();
extern void ISR8();
extern void ISR9();
extern void ISR10();
extern void ISR11();
extern void ISR12();
extern void ISR13();
extern void ISR14();
extern void ISR15();
extern void ISR16();
extern void ISR17();
extern void ISR18();
extern void ISR19();
extern void ISR20();
extern void ISR21();
extern void ISR22();
extern void ISR23();
extern void ISR24();
extern void ISR25();
extern void ISR26();
extern void ISR27();
extern void ISR28();
extern void ISR29();
extern void ISR30();
extern void ISR31();

extern void IRQ32();
extern void IRQ33();
extern void IRQ34();
extern void IRQ35();
extern void IRQ36();
extern void IRQ37();
extern void IRQ38();
extern void IRQ39();
extern void IRQ40();
extern void IRQ41();
extern void IRQ42();
extern void IRQ43();
extern void IRQ44();
extern void IRQ45();
extern void IRQ46();
extern void IRQ47();

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, error_code;
    uint32_t eip, cs, eflags, useresp, ss;
} register_t;
typedef void (*isr_t)(register_t);

void Install_ISR_And_IRQ();
void Install_Interrupt();
void Register_Interrupt(uint8_t number, isr_t handler);

#endif