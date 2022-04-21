#include <kernel/core/debug.h>
#include <kernel/core/kernel.h>

DEBUG       _debugmode;
terminal_t* _debugterm = NULL;

void  debug_setterm(terminal_t* term) { _debugterm = term; }

terminal_t* debug_getterm() { return _debugterm; }

void debug_setmode(DEBUG mode) { _debugmode = mode; _debugterm = NULL; }

DEBUG debug_getmode() { return _debugmode; }

void debug_halt() { asm volatile("cli"); asm volatile("hlt"); while (true); }

void debug_breakpoint() { asm volatile("xchg %bx, %bx"); }

void debug_newline() { debug_printc('\n'); }

void debug_printc(char c)
{
    if (_debugmode == DEBUG_SERIAL || _debugmode == DEBUG_ALL) { serial_printc(c); }
    if (_debugmode == DEBUG_TERMINAL || _debugmode == DEBUG_ALL) 
    { 
        if (_debugterm != NULL) { term_printc(_debugterm, c); }
    }
}

void debug_printc_fg(char c, uint32_t color)
{
    if (_debugmode == DEBUG_SERIAL || _debugmode == DEBUG_ALL) { serial_printc_fg(c, color); }
    if (_debugmode == DEBUG_TERMINAL || _debugmode == DEBUG_ALL) 
    { 
        if (_debugterm != NULL) { term_printc_fg(_debugterm, c, COL4ToCOL32(color)); }
    }
}

void debug_print(char* str)
{
    if (str == NULL) { return; }
    int i = 0;
    while (str[i] != 0) { debug_printc(str[i]); i++; }
}

void debug_print_fg(char* str, uint32_t color)
{
    if (str == NULL) { return; }
    int i = 0;
    while (str[i] != 0) { debug_printc_fg(str[i], color); i++; }
}

void debug_println(char* str)
{
    debug_print(str);
    debug_newline();
}

void debug_println_fg(char* str, uint32_t color)
{
    debug_print_fg(str, color);
    debug_newline();
}

void debug_vprintf(char* str, va_list args)
{
    if (_debugmode == DEBUG_SERIAL || _debugmode == DEBUG_ALL) { serial_vprintf(str, args); }
    if (_debugmode == DEBUG_TERMINAL || _debugmode == DEBUG_ALL) 
    { 
        if (_debugterm != NULL)
        {
            term_vprintf(_debugterm, str, args);
        }
    }
}

void debug_printf(char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_vprintf(str, args);
    va_end(args);
}

void debug_header(char* str, uint32_t color)
{
    debug_print("[");
    debug_print_fg(str, color);
    debug_print("] ");
}

void debug_info(char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  >>  ", COL4_CYAN);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_ok(char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  OK  ", COL4_GREEN);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_warning(char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  ??  ", COL4_YELLOW);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

void debug_error(char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  !!  ", COL4_RED);
    debug_vprintf(str, args);
    debug_newline();
    va_end(args);
}

const char* panic_msg(int irq)
{
    switch (irq)
    {
        case 0x00: { return "DIVIDE BY ZERO"; }
        default: { return "UNKNOWN ERROR"; }
    }
}

void kpanic(char* str, ...)
{
    va_list args;
    va_start(args, str);
    debug_header("  !!  ", COL4_RED);
    debug_vprintf(str, args);
    debug_newline();
    debug_error("THREAD - ID: %d, NAME: '%s'", THREAD->id, THREAD->name);
    va_end(args);
    debug_halt();
}

void kpanicf(char* str, int irq, ...)
{
    va_list args;
    va_start(args, irq);
    debug_header("  !!  ", COL4_RED);
    debug_vprintf(str, args);
    debug_newline();
    debug_error("THREAD - ID: %d, NAME: '%s'", THREAD->id, THREAD->name);
    if (irq < 0x20) { debug_println(panic_msg(irq)); }
    va_end(args);
    debug_halt();
}

void debug_hexdump(void* ptr, uint32_t size)
{
    
}