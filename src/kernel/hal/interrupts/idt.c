#include <kernel/hal/interrupts/idt.h>
#include <kernel/core/kernel.h>

#define KERNEL_CS 0x08

#define isr(x) extern void isr##x();
#define irq(x) extern void irq##x();

isr(0);  isr(1);  isr(2);  isr(3);  isr(4);  isr(5);  isr(6);  isr(7);
isr(8);  isr(9);  isr(10); isr(11); isr(12); isr(13); isr(14); isr(15);
isr(16); isr(17); isr(18); isr(19); isr(20); isr(21); isr(22); isr(23);
isr(24); isr(25); isr(26); isr(27); isr(28); isr(29); isr(30); isr(31);

irq(0);   irq(1); irq(2);  irq(3);  irq(4);  irq(5);  irq(6);  irq(7);
irq(8);   irq(9); irq(10); irq(11); irq(12); irq(13); irq(14); irq(15);
irq(128); 

extern void _idt_flush();
void idt_default_handler(irq_regs_t* regs) { }

const idt_attr_t IDT_EXCEPTION_ATTRS = (idt_attr_t){ .type = IDTTYPE_TRAP32, .dpl = 3, .present = 1 };
const idt_attr_t IDT_IRQ_ATTRS       = (idt_attr_t){ .type = IDTTYPE_INT32,  .dpl = 3, .present = 1 };

uint32_t      _idtr;
idtr_t        idtr ALIGNED(0x1000);
idt_entry_t   idt[IDT_COUNT] ALIGNED(0x1000);
irq_handler_t idt_handlers[IDT_COUNT];

void idt_init()
{
    memset(&idt, 0, sizeof(idt_entry_t) * IDT_COUNT);
    memset(&idt_handlers, 0, sizeof(irq_handler_t) * IDT_COUNT);

    idt_init_exceptions();

    port_outb(0x20, 0x11);
	port_outb(0xA0, 0x11);
	port_outb(0x21, 0x20);
	port_outb(0xA1, 0x28);
	port_outb(0x21, 0x04);
	port_outb(0xA1, 0x02);
	port_outb(0x21, 0x01);
	port_outb(0xA1, 0x01);
	port_outb(0x21, 0x0);
	port_outb(0xA1, 0x0);

    idt_init_irqs();

    _idtr      = ((uint32_t)&idtr);
    idtr.base  = ((uint32_t)&idt);
    idtr.limit = (IDT_COUNT * sizeof(idt_entry_t)) - 1;    

    _cli();
    _idt_flush();
    irq_register(IRQ7, idt_default_handler);
    debug_ok("Initialized IDT - LIST: 0x%8x, IDTR: 0x%8x", idtr.base, _idtr);
}

void idt_init_exceptions()
{
    idt_set_desc(0x00, (uint32_t)isr0,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x01, (uint32_t)isr1,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x02, (uint32_t)isr2,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x03, (uint32_t)isr3,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x04, (uint32_t)isr4,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x05, (uint32_t)isr5,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x06, (uint32_t)isr6,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x07, (uint32_t)isr7,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x08, (uint32_t)isr8,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x09, (uint32_t)isr9,  IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0A, (uint32_t)isr10, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0B, (uint32_t)isr11, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0C, (uint32_t)isr12, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0D, (uint32_t)isr13, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0E, (uint32_t)isr14, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x0F, (uint32_t)isr15, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x10, (uint32_t)isr16, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x11, (uint32_t)isr17, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x12, (uint32_t)isr18, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x13, (uint32_t)isr19, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x14, (uint32_t)isr20, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x15, (uint32_t)isr21, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x16, (uint32_t)isr22, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x17, (uint32_t)isr23, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x18, (uint32_t)isr24, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x19, (uint32_t)isr25, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1A, (uint32_t)isr26, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1B, (uint32_t)isr27, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1C, (uint32_t)isr28, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1D, (uint32_t)isr29, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1E, (uint32_t)isr30, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    idt_set_desc(0x1F, (uint32_t)isr31, IDT_EXCEPTION_ATTRS, KERNEL_CS);
    debug_info("Initialized exception descriptors");
}

void idt_init_irqs()
{
    idt_set_desc(0x20, (uint32_t)irq0,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x21, (uint32_t)irq1,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x22, (uint32_t)irq2,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x23, (uint32_t)irq3,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x24, (uint32_t)irq4,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x25, (uint32_t)irq5,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x26, (uint32_t)irq6,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x27, (uint32_t)irq7,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x28, (uint32_t)irq8,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x29, (uint32_t)irq9,  IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2A, (uint32_t)irq10, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2B, (uint32_t)irq11, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2C, (uint32_t)irq12, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2D, (uint32_t)irq13, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2E, (uint32_t)irq14, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x2F, (uint32_t)irq15, IDT_IRQ_ATTRS, KERNEL_CS);
    idt_set_desc(0x80, (uint32_t)irq128, IDT_IRQ_ATTRS, KERNEL_CS);
    debug_info("Initialized IRQ descriptors");
}

void idt_set_desc(uint8_t n, uint32_t base, idt_attr_t attributes, uint16_t segment)
{
	idt[n].offset_low         = LOW_16(base);
	idt[n].offset_high        = HIGH_16(base);
	idt[n].reserved           = 0;
    idt[n].attributes.type    = attributes.type;
    idt[n].attributes.dpl     = attributes.dpl;
    idt[n].attributes.present = attributes.present;
    idt[n].attributes.unused  = 0;
	idt[n].segment            = segment;
}

void irq_register(IRQ irq, irq_handler_t handler)
{
    idt_handlers[irq] = handler;
    if (irq >= IRQ0 + 8) { port_outb(0xA0, 0x20); }
    port_outb(0x20, 0x20);

    debug_info("Registered interrupt - IRQ: 0x%2x, HANDLER: 0x%8x", irq - IRQ0, handler);
}

void irq_unregister(IRQ irq)
{
    idt_handlers[irq] = NULL;
    debug_info("Unregistered interrupt - IRQ: 0x%2x", irq - IRQ0);
}

void exception_handler(irq_regs_t* regs)
{
    kpanicf("CPU EXCEPTION 0x%2x", regs->irq, regs->irq);
    debug_halt();
}

void irq_handler(irq_regs_t* regs)
{
    if (idt_handlers[regs->irq] != NULL) { idt_handlers[regs->irq](regs); }
    else if (regs->irq > IRQ0) { debug_error("Unhandled interrupt: 0x%2x", regs->irq - IRQ0); }
}

void irq_ack(irq_regs_t* regs)
{
    if (regs->irq >= IRQ0 + 8) { port_outb(0xA0, 0x20); }
    port_outb(0x20, 0x20);
}