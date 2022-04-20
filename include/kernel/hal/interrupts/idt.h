#pragma once
#include <lib/stdint.h>

typedef enum
{
    IRQ0 = 32,
    IRQ1,
    IRQ2,
    IRQ3,
    IRQ4,
    IRQ5,
    IRQ6,
    IRQ7,
    IRQ8,
    IRQ9,
    IRQ10,
    IRQ11,
    IRQ12,
    IRQ13,
    IRQ14,
    IRQ15,
    IRQ_SYSCALL = 160,
} IRQ;

typedef enum
{
    IDTTYPE_TASK   = 0x05,
    IDTTYPE_INT16  = 0x06,
    IDTTYPE_INT32  = 0x0E,
    IDTTYPE_TRAP16 = 0x07,
    IDTTYPE_TRAP32 = 0x0F,
} IDTTYPE;

typedef struct
{
    IDTTYPE type    : 4;
    uint8_t unused  : 1;
    uint8_t dpl     : 2;
    uint8_t present : 1;
} PACKED idt_attr_t;

typedef struct
{
    uint16_t   offset_low;
    uint16_t   segment;
    uint8_t    reserved;
    idt_attr_t attributes;
    uint16_t   offset_high;
} PACKED idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} PACKED idtr_t;

typedef struct
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t irq, err;
    uint32_t eip, cs, eflags, useresp, ss;
} irq_regs_t;

typedef void (*irq_handler_t)(irq_regs_t*);

#define IDT_COUNT 256

extern uint32_t _idtr;

void idt_init();
void idt_init_exceptions();
void idt_init_irqs();
void idt_set_desc(uint8_t n, uint32_t base, idt_attr_t attributes, uint16_t segment);

void irq_register(IRQ irq, irq_handler_t handler);
void irq_unregister(IRQ irq);

void irq_ack(irq_regs_t* regs);

static inline void _hlt() { asm volatile("hlt"); }
static inline void _cli() { asm volatile("cli"); }
static inline void _sti() { asm volatile("sti"); }