#pragma once
#include <lib/stdint.h>
#include <kernel/hal/interrupts/idt.h>

void ps2_init();
int ps2_wait_input();
int ps2_wait_output();
void ps2_set_smplrate(uint8_t rate);
void ps2_sendcmd(uint8_t cmd);
void ps2_sendcmd_ext(uint8_t cmd, uint8_t arg);
uint8_t ps2_cmdresp(uint8_t cmd);
void ps2_write_ms(uint8_t data);
void ps2_write_kbd(uint8_t data);
void ps2_callback_shared(irq_regs_t* regs);
void ps2_callback_ms(irq_regs_t* regs);
void ps2_callback_kbd(irq_regs_t* regs);