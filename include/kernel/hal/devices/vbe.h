#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <kernel/services/video.h>

void vbe_init();
void vbe_setmode(int w, int h);
void vbe_disable();
videomode_t* vbe_fetchmodes(int* count);
uint32_t* vbe_getbuff();
void vbe_newline();
void vbe_delete();
void vbe_scroll();
void vbe_clear(uint32_t color);
void vbe_blit(uint8_t x, uint8_t y, uint32_t color);
void vbe_putc(uint8_t x, uint8_t y, char c, uint32_t fg, uint32_t bg);
void vbe_putstr(uint8_t x, uint8_t y, char* str, uint32_t fg, uint32_t bg);
void vbe_printc(char c);
void vbe_printc_fg(char c, uint32_t fg);
void vbe_printc_fbg(char c, uint32_t fg, uint32_t bg);
void vbe_print(char* str);
void vbe_print_fg(char* str, uint32_t fg);
void vbe_print_fbg(char* str, uint32_t fg, uint32_t bg);
void vbe_println(char* str);
void vbe_println_fg(char* str, uint32_t fg);
void vbe_println_fbg(char* str, uint32_t fg, uint32_t bg);
void vbe_vprintf(char* str, va_list args);
void vbe_printf(char* str, ...);

void vbe_enablecur(uint8_t top, uint8_t bottom);
void vbe_disablecur();
void vbe_setcur(uint8_t x, uint8_t y);
void vbe_setcur_x(uint8_t x);
void vbe_setcur_y(uint8_t y);
uint8_t vbe_getcur_x();
uint8_t vbe_getcur_y();

void vbe_setfg(uint32_t fg);
void vbe_setbg(uint32_t bg);
uint8_t vbe_getfg();
uint8_t vbe_getbg();