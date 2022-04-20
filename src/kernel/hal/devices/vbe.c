#include <kernel/hal/devices/vbe.h>
#include <kernel/core/kernel.h>

#define LNG_PTR(seg, off) ((seg << 4) | off)
#define REAL_PTR(arr) LNG_PTR(arr[1], arr[0])
#define SEG(addr) (((uint32_t)addr >> 4) & 0xF000)
#define OFF(addr) ((uint32_t)addr & 0xFFFF)

vbe_ctrl_info_t* _vbe_ctrlinfo;
vbe_mode_info_t  _vbe_modeinfo;
bool             _vbe_enabled = false;
uint32_t         _vbe_modecount;

void vbe_populate();

void vbe_init()
{
    _vbe_modecount = 0;
    _vbe_enabled   = true;
    vbe_setmode(KARGS.video_mode.width, KARGS.video_mode.height);

    debug_ok("Initialized VBE controller");
}

void vbe_setmode(int w, int h)
{
    vbe_populate();
    if (w == _vbe_modeinfo.width && h == _vbe_modeinfo.height) { return; }

    regs16_t regs;
    vbe_mode_info_t* mode_info = (0x80000 + sizeof(vbe_ctrl_info_t) + 512);
    uint16_t* modes = REAL_PTR(_vbe_ctrlinfo->videomode);
    _vbe_modecount = 0;

    uint16_t mode;
    bool set = false;
    for (int i = 0; modes[i] != 0xFFFF; i++)
    {
        regs.ax = 0x4f01;
        regs.cx = modes[i];
        regs.es = SEG(mode_info);
        regs.di = OFF(mode_info);
        
        int16(0x10, &regs);

        if (mode_info->width == w && mode_info->height == h && mode_info->depth == 32 && !set)
        {
            set = true;
            mode = modes[i];
            _vbe_modeinfo = *mode_info;
        }
        if (mode_info->width > 0 && mode_info->depth == 32) { _vbe_modecount++; }
    }

    if (!set) { debug_error("Invalid VBE mode - %dx%dx32bpp", w, h); return; }

    regs.ax = 0x4F02;
    regs.bx = mode | 0x4000;
    int16(0x10, &regs);
}

void vbe_disable()
{

}

void vbe_populate()
{
    vbe_ctrl_info_t* info = 0x80000;

    regs16_t regs;
    regs.ax = 0x4f00;
    regs.es = 0x8000;
    regs.di = 0x0;

    int16(0x10, &regs);
    _vbe_ctrlinfo = info;
}

videomode_t* vbe_fetchmodes(int* count)
{
    videomode_t* output = tmalloc(_vbe_modecount * sizeof(videomode_t), MEMTYPE_ARRAY);

    regs16_t regs;
    vbe_mode_info_t* mode_info = (0x80000 + sizeof(vbe_ctrl_info_t) + 512);
    uint16_t* modes = REAL_PTR(_vbe_ctrlinfo->videomode);

    uint16_t mode, pos = 0;
    for (int i = 0; modes[i] != 0xFFFF; i++)
    {
        regs.ax = 0x4f01;
        regs.cx = modes[i];
        regs.es = SEG(mode_info);
        regs.di = OFF(mode_info);      
        int16(0x10, &regs);

        if (mode_info->width != 0 && mode_info->height != 0 && mode_info->depth == 32)
        {
            output[pos] = (videomode_t){ .width = mode_info->width, .height = mode_info->height, .depth = 32 };
            pos++;
            mode        = modes[i];
        }
    }
    *count = pos;
    return output;
}

uint32_t* vbe_getbuff() { return _vbe_modeinfo.physical_base; }

void vbe_newline()
{

}

void vbe_delete()
{

}

void vbe_scroll()
{

}

void vbe_clear(uint32_t color)
{
    size_t l = _vbe_modeinfo.width * _vbe_modeinfo.height;
    asm volatile("cld;rep stosl" : "+D"(_vbe_modeinfo.physical_base), "+c"(l) : "a"(color) : "memory");
}

void vbe_blit(uint8_t x, uint8_t y, uint32_t color)
{

}

void vbe_putc(uint8_t x, uint8_t y, char c, uint32_t fg, uint32_t bg)
{
    
}

void vbe_putstr(uint8_t x, uint8_t y, char* str, uint32_t fg, uint32_t bg)
{

}

void vbe_printc(char c)
{

}

void vbe_printc_fg(char c, uint32_t fg)
{

}

void vbe_printc_fbg(char c, uint32_t fg, uint32_t bg)
{

}

void vbe_print(char* str)
{

}

void vbe_print_fg(char* str, uint32_t fg)
{

}

void vbe_print_fbg(char* str, uint32_t fg, uint32_t bg)
{

}

void vbe_println(char* str)
{

}

void vbe_println_fg(char* str, uint32_t fg)
{

}

void vbe_println_fbg(char* str, uint32_t fg, uint32_t bg)
{

}

void vbe_vprintf(char* str, va_list args)
{

}

void vbe_printf(char* str, ...)
{
    
}

void vbe_enablecur(uint8_t top, uint8_t bottom)
{

}

void vbe_disablecur()
{

}

void vbe_setcur(uint8_t x, uint8_t y)
{

}

void vbe_setcur_x(uint8_t x)
{

}

void vbe_setcur_y(uint8_t y)
{

}

uint8_t vbe_getcur_x() { }

uint8_t vbe_getcur_y() { }

void vbe_setfg(uint32_t fg)
{

}

void vbe_setbg(uint32_t bg)
{

}

uint8_t vbe_getfg() { }

uint8_t vbe_getbg() { }