#pragma once
#include <lib/stdint.h>
#include <lib/time.h>
#include <kernel/hal/interrupts/idt.h>

void rtc_callback(irq_regs_t* regs);
void rtc_init();
void rtc_read();
time_t rtc_get_time();