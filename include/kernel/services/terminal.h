#pragma once
#include <lib/stdint.h>
#include <lib/gui/window.h>
#include <lib/gui/widget.h>
#include <kernel/hal/devices/keyboard.h>

typedef struct
{
    gui_window_t        window;
    uint32_t            time, timelast, curtime;
    uint32_t            width, height;
    uint32_t            cx, cy;
    uint32_t            fg, bg;
    uint32_t            old_w, old_h;
    image_t             buffer;
    char*               path;
    kbd_handler_t  kb;
} PACKED terminal_t;

terminal_t* term_create(int x, int y);

void term_update(terminal_t* term);
void term_draw(terminal_t* term);
void term_render(terminal_t* term);

void term_clear(terminal_t* term);
void term_clear_col(terminal_t* term, uint32_t color);

void term_newline(terminal_t* term);
void term_delete(terminal_t* term);
void term_scroll(terminal_t* term);

void term_printcaret(terminal_t* term);

void term_putc(terminal_t* term, int x, int y, char c, uint32_t fg, uint32_t bg);

void term_printc(terminal_t* term, char c);
void term_printc_fg(terminal_t* term, char c, uint32_t fg);
void term_printc_fbg(terminal_t* term, char c, uint32_t fg, uint32_t bg);

void term_print(terminal_t* term, char* str);
void term_print_fg(terminal_t* term, char* str, uint32_t fg);
void term_print_fbg(terminal_t* term, char* str, uint32_t fg, uint32_t bg);

void term_println(terminal_t* term, char* str);
void term_println_fg(terminal_t* term, char* str, uint32_t fg);
void term_println_fbg(terminal_t* term, char* str, uint32_t fg, uint32_t bg);

void term_vprintf(terminal_t* term, char* str, va_list args);
void term_printf(terminal_t* term, char* str, ...);

char* term_getfullpath(terminal_t* term, char* path);