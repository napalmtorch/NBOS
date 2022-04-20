#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>

typedef enum
{
    SERIALPORT_NONE = 0x000,
    SERIALPORT_COM1 = 0x3F8,
    SERIALPORT_COM2 = 0x2F8,
    SERIALPORT_COM3 = 0x3E8,
    SERIALPORT_COM4 = 0x2E8,
} SERIALPORT;

void        serial_setport(SERIALPORT port);
SERIALPORT  serial_getport();
const char* serial_getportstr(SERIALPORT port);

char serial_read();
void serial_printc(char c);
void serial_printc_fg(char c, uint8_t color);
void serial_print(const char* str);
void serial_print_fg(const char* str, uint8_t color);
void serial_println(const char* str);
void serial_println_fg(const char* str, uint8_t color);
void serial_newline();
int  serial_vprintf(const char* str, va_list args);
int  serial_printf(const char* str, ...);
void serial_setcol(uint8_t color);