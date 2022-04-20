#pragma once
#include <lib/stdint.h>
#include <lib/stddef.h>
#include <lib/stdarg.h>

static inline uint8_t port_inb(uint16_t port)
{
    uint8_t result;
    asm volatile("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
}

static inline uint16_t port_inw(uint16_t port)
{
    uint16_t result;
    asm volatile("in %%dx, %%ax" : "=a" (result) : "d" (port));
    return result;
}

static inline uint32_t port_inl(uint16_t port)
{
    uint32_t result;
    asm volatile("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
    return result;
}

static inline void port_instr(uint16_t port, uint8_t* data, uint32_t size) { asm volatile("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory"); }

static inline void port_outb(uint16_t port, uint8_t data) { asm volatile("out %%al, %%dx" : : "a" (data), "d" (port)); }
static inline void port_outw(uint16_t port, uint16_t data) { asm volatile("out %%ax, %%dx" : : "a" (data), "d" (port)); }
static inline void port_outl(uint16_t port, uint32_t data) { asm volatile("outl %%eax, %%dx" : : "dN" (port), "a" (data)); }
static inline void port_outstr(uint16_t port, uint8_t* data, uint32_t size) { asm volatile("rep outsw" : "+S" (data), "+c" (size) : "d" (port)); }