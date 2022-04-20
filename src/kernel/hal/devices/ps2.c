#include <kernel/hal/devices/ps2.h>
#include <kernel/core/kernel.h>

#define PS2_DATA           0x60
#define PS2_STATUS         0x64
#define PS2_COMMAND        0x64
#define MOUSE_WRITE        0xD4
#define MOUSE_V_BIT        0x08

#define PS2_PORT1_IRQ      0x01
#define PS2_PORT2_IRQ      0x02
#define PS2_PORT1_TLATE    0x40

#define PS2_READ_CONFIG    0x20
#define PS2_WRITE_CONFIG   0x60

#define PS2_DISABLE_PORT2  0xA7
#define PS2_ENABLE_PORT2   0xA8
#define PS2_DISABLE_PORT1  0xAD
#define PS2_ENABLE_PORT1   0xAE

#define MOUSE_SET_REMOTE   0xF0
#define MOUSE_DEVICE_ID    0xF2
#define MOUSE_SAMPLE_RATE  0xF3
#define MOUSE_DATA_ON      0xF4
#define MOUSE_DATA_OFF     0xF5
#define MOUSE_SET_DEFAULTS 0xF6

#define MOUSE_DEFAULT         0
#define MOUSE_SCROLLWHEEL     1
#define MOUSE_BUTTONS         2

#define KBD_SET_SCANCODE   0xF0

void ps2_init()
{
    _cli();
    ps2_sendcmd(PS2_DISABLE_PORT1);
    ps2_sendcmd(PS2_DISABLE_PORT2);

    while ((port_inb(PS2_STATUS) & 1)) { port_inb(PS2_DATA); }

    uint8_t status = ps2_cmdresp(PS2_READ_CONFIG);
    status |= (PS2_PORT1_IRQ | PS2_PORT2_IRQ | PS2_PORT1_TLATE);
    ps2_sendcmd_ext(PS2_WRITE_CONFIG, status);

    ps2_sendcmd(PS2_ENABLE_PORT1);
    ps2_sendcmd(PS2_ENABLE_PORT2);
    debug_info("Enabled PS/2 ports");

    irq_register(IRQ1, ps2_callback_shared);
    irq_register(IRQ12, ps2_callback_shared);
    debug_info("Registered PS/2 interrupt callbacks");

    ps2_write_kbd(KBD_SET_SCANCODE);
    ps2_write_kbd(2);
    debug_info("Set PS/2 keyboard scancode to 2");

    ps2_write_ms(MOUSE_SET_DEFAULTS);
    ps2_write_ms(MOUSE_DATA_ON);
    debug_info("Configured PS/2 mouse with default settings");

    _sti();
    debug_ok("Finished initialized PS/2 controller");

}

int ps2_wait_input()
{
    uint32_t timeout = 100000;
    while (--timeout) { if (!(port_inb(PS2_STATUS) & (1 << 1))) { return 0; } }
    return 1;
}

int ps2_wait_output()
{
    uint32_t timeout = 100000;
    while (--timeout) { if (port_inb(PS2_STATUS) & (1 << 0)) { return 0; } }
    return 1;
}

void ps2_set_smplrate(uint8_t rate)
{
    ps2_write_ms(MOUSE_SAMPLE_RATE);
    while (!(port_inb(PS2_STATUS) & 1));
    port_inb(PS2_DATA);
    ps2_write_ms(rate);
    while (!(port_inb(PS2_STATUS) & 1));
    port_inb(PS2_DATA);
}

void ps2_sendcmd(uint8_t cmd)
{
    ps2_wait_input();
    port_outb(PS2_COMMAND, cmd);
}

void ps2_sendcmd_ext(uint8_t cmd, uint8_t arg)
{
    ps2_wait_input();
    port_outb(PS2_COMMAND, cmd);
    ps2_wait_input();
    port_outb(PS2_DATA, arg);
}

uint8_t ps2_cmdresp(uint8_t cmd)
{
    ps2_wait_input();
    port_outb(PS2_COMMAND, cmd);
    ps2_wait_output();
    return port_inb(PS2_DATA);
}

void ps2_write_ms(uint8_t data) { ps2_sendcmd_ext(MOUSE_WRITE, data); }

void ps2_write_kbd(uint8_t data)
{
    ps2_wait_input();
    port_outb(PS2_DATA, data);
}

void ps2_callback_shared(irq_regs_t* regs)
{
    ps2_sendcmd(PS2_DISABLE_PORT1);
    ps2_sendcmd(PS2_DISABLE_PORT2);

    uint8_t status = port_inb(PS2_STATUS);
    uint8_t data   = port_inb(PS2_DATA);

    ps2_sendcmd(PS2_ENABLE_PORT1);
    ps2_sendcmd(PS2_ENABLE_PORT2);

    irq_ack(regs);

    if (!(status & 0x01)) { return; }
    if (!(status & 0x20)) { kbd_handle(data); }
    else if (status & 0x21) { mouse_handle(data); }
}

void ps2_callback_ms(irq_regs_t* regs)
{
    uint8_t status = port_inb(PS2_STATUS);
    uint8_t data   = port_inb(PS2_DATA);

    irq_ack(regs);
    if (!(status & 0x01)) { return; }
    mouse_handle(data);
}

void ps2_callback_kbd(irq_regs_t* regs)
{   
    uint8_t status = port_inb(PS2_STATUS);
    uint8_t data   = port_inb(PS2_DATA);

    irq_ack(regs);
    if (!(status & 0x01)) { return; }
    kbd_handle(data);
}