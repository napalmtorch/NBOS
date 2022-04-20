#include <kernel/hal/devices/vga.h>
#include <kernel/core/kernel.h>

#define VGA_BASE 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

uint8_t* _vga_base;
uint8_t  _vga_w, _vga_h, _vga_cx, _vga_cy;
uint8_t  _vga_fg, _vga_bg;
bool     _vga_enabled = false;

void vga_init()
{
    if (_vga_enabled) { return; }

    _vga_base = VGA_BASE;
    _vga_w    = VGA_WIDTH;
    _vga_h    = VGA_HEIGHT;
    _vga_fg   = COL4_WHITE;
    _vga_bg   = COL4_BLACK;
    _vga_enabled = true;

    vga_enablecur(0, 15);
    vga_clear(_vga_bg);

    debug_ok("Initialized VGA controller");
}

void vga_disable()
{
    vga_disablecur();
    vga_clear(COL4_BLACK);
    _vga_enabled = false;
}

void vga_newline()
{
    if (!_vga_enabled) { return; }

    vga_setcur(0, _vga_cy + 1);

    if (_vga_cy >= _vga_h) { vga_scroll(); }
}

void vga_delete()
{
    if (!_vga_enabled) { return; }
}

void vga_scroll()
{
    if (!_vga_enabled) { return; }
    vga_setcur(0, _vga_h - 1);
}

void vga_clear(uint8_t color)
{
    if (!_vga_enabled) { return; }

    _vga_bg = color;
    for (int i = 0; i < _vga_w * _vga_h * 2; i += 2)
    {
        _vga_base[i + 0] = 0x20;
        _vga_base[i + 1] = (color << 4) | _vga_fg;
    }
    vga_setcur(0, 0);
}

void vga_blit(uint8_t x, uint8_t y, uint8_t color) { vga_putc(x, y, 0x20, _vga_fg, color); }

void vga_putc(uint8_t x, uint8_t y, char c, uint8_t fg, uint8_t bg)
{
    if (!_vga_enabled || x >= _vga_w || y >= _vga_h) { return; }
    _vga_base[((y * _vga_w + x) * 2) + 0] = c;
    _vga_base[((y * _vga_w + x) * 2) + 1] = (bg << 4) | fg;
}

void vga_putstr(uint8_t x, uint8_t y, char* str, uint8_t fg, uint8_t bg)
{
    if (!_vga_enabled || str == NULL) { return; }

    int xx = x, yy = y, i = 0;
    while (str[i] != 0)
    {
        if (str[i] == '\n') { xx = x; yy++; }
        else { vga_putc(xx, yy, str[i], fg, bg); }
    }
}

void vga_printc(char c) { vga_printc_fbg(c, _vga_fg, _vga_bg); }

void vga_printc_fg(char c, uint8_t fg) { vga_printc_fbg(c, fg, _vga_bg); }

void vga_printc_fbg(char c, uint8_t fg, uint8_t bg)
{
    if (!_vga_enabled) { return; }
    if (c == '\n') { vga_newline(); return; }
    vga_putc(_vga_cx, _vga_cy, c, fg, bg);
    _vga_cx++;
    if (_vga_cx >= _vga_w) { vga_newline(); return; }
    vga_setcur(_vga_cx, _vga_cy);
}

void vga_print(char* str) { vga_print_fbg(str, _vga_fg, _vga_bg); }

void vga_print_fg(char* str, uint8_t fg) { vga_print_fbg(str, fg, _vga_bg); }

void vga_print_fbg(char* str, uint8_t fg, uint8_t bg)
{
    if (!_vga_enabled || str == NULL) { return; }
    int i = 0;
    while (str[i] != 0) { vga_printc_fbg(str[i], fg, bg); i++; }
}

void vga_println(char* str)
{
    vga_print(str);
    vga_newline();
}

void vga_println_fg(char* str, uint8_t fg)
{
    vga_print_fg(str, fg);
    vga_newline();
}

void vga_println_fbg(char* str, uint8_t fg, uint8_t bg)
{
    vga_print_fbg(str, fg, bg);
    vga_newline();
}

void vga_vprintf(char* str, va_list args)
{
    while (*str != 0)
    {
        if (*str == '%')
        {
            str++;
            if (*str == '%') { vga_printc('%'); }
            if (*str == 'c') { vga_printc((char)va_arg(args, int)); }
            else if (*str == 'd' || *str == 'u')
            {
                int dec = va_arg(args, int);
                char str[16];
                memset(str, 0, 16);
                vga_print(itoa(dec, str, 10));
            }
            else if (*str == 'u')
            {
                uint32_t dec = va_arg(args, uint32_t);
                char str[16];
                memset(str, 0, 16);
                vga_print(ltoa(dec, str, 10));
            }
            else if (*str == 'x')
            {
                uint32_t dec = va_arg(args, uint32_t);
                char str[16];
                memset(str, 0, 16);
                vga_print(ltoa(dec, str, 16));
            }
            else if (*str == '2')
            {
                str++;
                if (*str != 'x' && *str != 'X') { vga_printc(*str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    vga_print(strhex(num, str, false, 1));
                }
            }
            else if (*str == '4')
            {
                str++;
                if (*str != 'x' && *str != 'X') { vga_printc(*str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    vga_print(strhex(num, str, false, 2));
                }
            }
            else if (*str == '8')
            {
                str++;
                if (*str != 'x' && *str != 'X') { vga_printc(*str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    vga_print(strhex(num, str, false, 4));
                }
            }
            else if (*str == 'f') { }
            else if (*str == 's')
            {
                char* val = va_arg(args, char*);
                vga_print(val);
            }
            else { vga_printc(*str); }
        }
        else { vga_printc(*str); }
        str++;
    }
}

void vga_printf(char* str, ...)
{
    va_list args;
    va_start(args, str);
    vga_vprintf(str, args);
    va_end(args);
}

void vga_enablecur(uint8_t top, uint8_t bottom)
{
    port_outb(0x3D4, 0x0A);
	port_outb(0x3D5, (port_inb(0x3D5) & 0xC0) | top);
 
	port_outb(0x3D4, 0x0B);
	port_outb(0x3D5, (port_inb(0x3D5) & 0xE0) | bottom);
}

void vga_disablecur()
{
    port_outb(0x3D4, 0x0A);
	port_outb(0x3D5, 0x20);
}

void vga_setcur(uint8_t x, uint8_t y)
{
    _vga_cx = x;
    _vga_cy = y;
    vga_updatecur();
}

void vga_setcur_x(uint8_t x) { _vga_cx = x; vga_updatecur(); }

void vga_setcur_y(uint8_t y) { _vga_cy = y; vga_updatecur(); }

void vga_updatecur()
{
    uint16_t pos = _vga_cy * VGA_WIDTH + _vga_cx;
	port_outb(0x3D4, 0x0F);
	port_outb(0x3D5, (uint8_t) (pos & 0xFF));
	port_outb(0x3D4, 0x0E);
	port_outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
} 

uint8_t vga_getcur_x() { return _vga_cx; }

uint8_t vga_getcur_y() { return _vga_cy; }

void vga_setfg(uint8_t fg) { _vga_fg = fg; }

void vga_setbg(uint8_t bg) { _vga_bg = bg; }

uint8_t vga_getfg() { return _vga_fg; }

uint8_t vga_getbg() { return _vga_bg; }