#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>

void vga_init();
void vga_disable();
void vga_newline();
void vga_delete();
void vga_scroll();
void vga_clear(uint8_t color);
void vga_blit(uint8_t x, uint8_t y, uint8_t color);
void vga_putc(uint8_t x, uint8_t y, char c, uint8_t fg, uint8_t bg);
void vga_putstr(uint8_t x, uint8_t y, char* str, uint8_t fg, uint8_t bg);
void vga_printc(char c);
void vga_printc_fg(char c, uint8_t fg);
void vga_printc_fbg(char c, uint8_t fg, uint8_t bg);
void vga_print(char* str);
void vga_print_fg(char* str, uint8_t fg);
void vga_print_fbg(char* str, uint8_t fg, uint8_t bg);
void vga_println(char* str);
void vga_println_fg(char* str, uint8_t fg);
void vga_println_fbg(char* str, uint8_t fg, uint8_t bg);
void vga_vprintf(char* str, va_list args);
void vga_printf(char* str, ...);

void vga_enablecur(uint8_t top, uint8_t bottom);
void vga_disablecur();
void vga_setcur(uint8_t x, uint8_t y);
void vga_setcur_x(uint8_t x);
void vga_setcur_y(uint8_t y);
void vga_updatecur();
uint8_t vga_getcur_x();
uint8_t vga_getcur_y();

void vga_setfg(uint8_t fg);
void vga_setbg(uint8_t bg);
uint8_t vga_getfg();
uint8_t vga_getbg();