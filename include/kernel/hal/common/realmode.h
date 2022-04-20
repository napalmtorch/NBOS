#pragma once
#include <lib/stdint.h>

typedef struct
{
	uint16_t di, si, bp, sp, bx, dx, cx, ax;
	uint16_t gs, fs, es, ds, eflags;
} PACKED regs16_t;

extern void int16(uint8_t interrupt, regs16_t* regs);