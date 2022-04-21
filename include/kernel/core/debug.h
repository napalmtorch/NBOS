#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <lib/stddef.h>
#include <kernel/services/terminal.h>

typedef enum
{
    DEBUG_DISABLED,
    DEBUG_SERIAL,
    DEBUG_TERMINAL,
    DEBUG_ALL,
} DEBUG;

void  debug_setterm(terminal_t* term);
terminal_t* debug_getterm();

void  debug_setmode(DEBUG mode);
DEBUG debug_getmode();

void debug_halt();
void debug_breakpoint();

void debug_newline();
void debug_printc(char c);
void debug_printc_fg(char c, uint32_t color);
void debug_print(char* str);
void debug_print_fg(char* str, uint32_t color);
void debug_println(char* str);
void debug_println_fg(char* str, uint32_t color);
void debug_vprintf(char* str, va_list args);
void debug_printf(char* str, ...);

void debug_header(char* str, uint32_t color);
void debug_info(char* str, ...);
void debug_ok(char* str, ...);
void debug_warning(char* str, ...);
void debug_error(char* str, ...);
void kpanic(char* str, ...);
void kpanicf(char* str, int irq, ...);

void debug_hexdump(void* ptr, uint32_t size);