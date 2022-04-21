#include <kernel/services/terminal.h>
#include <kernel/core/kernel.h>

void term_onchar(terminal_t* term, int c);
void term_onenter(terminal_t* term, void* unused);
void term_onback(terminal_t* term, void* unused);

terminal_t* term_create(int x, int y)
{
    int w = 720, h = 480;
    terminal_t* term                = gui_create_window(x, y, w, h, "Terminal");
    term->window.base.base.update   = term_update;
    term->window.base.base.draw     = term_draw;
    term->window.base.base.render   = term_render;
    term->window.winflags.show_icon = true;
    term->window.icon               = ASSETS.icons16;
    term->window.icon_x             = 64;
    term->window.icon_y             = 48;
    
    term->kb        = (kbd_handler_t){ .on_char = term_onchar, .on_enter = term_onenter, .on_backspace = term_onback };
    term->kb.stream = stream_create(4096);
    term->kb.host   = term;

    term->width                     = (w - (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 3 : 2)) / font_getw(&FONT_SANS8x14);
    term->height                    = (h - (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 3 : 2) - 19) / font_geth(&FONT_SANS8x14);
    term->old_w                     = w;
    term->old_h                     = h;
    term->buffer                    = image_create(term->width * font_getw(&FONT_SANS8x14), term->height * font_geth(&FONT_SANS8x14));
    term->fg                        = COL32_WHITE;
    term->bg                        = COL32_BLACK;
    term_clear(term);
    term_println_fg(term, "Napalmtorch Terminal", COL32_CYAN);
    term_println(term, "Version 1.0");
    term_println_fg(term, "All rights reserved", COL32_GRAY);
    term_printcaret(term);
    debug_info("Created terminal at 0x%8x", term);
    return term;
}

void term_update(terminal_t* term)
{
    gui_update_window(term);
    term->window.base.base.theme.colors[THEME_BG] = term->bg;

    if (term->window.base.base.flags.active) { kbd_set_handler(&term->kb); }
    else if (kbd_get_handler() == &term->kb) { kbd_set_handler(NULL); }

    if (term->window.base.base.bounds.width != term->old_w || term->window.base.base.bounds.height != term->old_h)
    {
        term->old_w = term->window.base.base.bounds.width;
        term->old_h = term->window.base.base.bounds.height;
        term->width                     = (term->old_w - (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 3 : 2)) / font_getw(&FONT_SANS8x14);
        term->height                    = (term->old_h - (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 3 : 2) - 19) / font_geth(&FONT_SANS8x14);
        image_dispose(&term->buffer);
        term->buffer                    = image_create(term->width * font_getw(&FONT_SANS8x14), term->height * font_geth(&FONT_SANS8x14));
        term_clear(term);
        term_printcaret(term);
    }
}

void term_draw(terminal_t* term)
{
    gui_draw_window(term);
    int bordl = (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 1 : 0);
    image_copy(&term->window.base.buffer, bordl, bordl + 19, term->buffer.width, term->buffer.height, term->buffer.data);
}

void term_render(terminal_t* term)
{
    gui_render_window(term);

    term->time = pit_get_millis();
    if (term->time != term->timelast) { term->timelast = term->time; term->curtime++; }

    if (term->curtime < 500)
    {
        int bordl = (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 1 : 0);
        image_rectf(&term->window.base.buffer, bordl + (term->cx * font_getw(&FONT_SANS8x14)), bordl + 19 + (term->cy * font_geth(&FONT_SANS8x14)), font_getw(&FONT_SANS8x14), font_geth(&FONT_SANS8x14), term->fg);
    }
    if (term->curtime >= 500 && term->curtime < 505) { term_draw(term); }
    if (term->curtime >= 1000) { term->curtime = 0; }
}

void term_clear(terminal_t* term)
{
    if (term == NULL) { return; }
    image_clear(&term->buffer, term->bg);
    term->cx = 0;
    term->cy = 0;
    term_draw(term);
}

void term_clear_col(terminal_t* term, uint32_t color)
{
    if (term == NULL) { return; }
    term->bg = color;
    term_clear(term);
}

void term_newline(terminal_t* term)
{
    if (term == NULL) { return; }
    term->cx = 0;
    term->cy++;
    if (term->cy >= term->height) { term_scroll(term); return; }
    term_draw(term);
}

void term_delete(terminal_t* term)
{
    if (term == NULL) { return; }

    if (term->cx > 0)
    {
        term->cx--;
        term_putc(term, term->cx, term->cy, 0x20, term->fg, term->bg);
    }
    else if (term->cy > 0)
    {
        term->cx = term->width - 1;
        term->cy--;
        term_putc(term, term->cx, term->cy, 0x20, term->fg, term->bg);
    }
}

void term_scroll(terminal_t* term)
{
    if (term == NULL) { return; }

    uint32_t line = (term->buffer.width * font_geth(&FONT_SANS8x14)) * 4;
    uint32_t size = term->buffer.width * term->buffer.height * 4;
    memcpy(term->buffer.data, (void*)((uint32_t)term->buffer.data + line), size - line);
    for (int i = 0; i < term->width; i++) { term_putc(term, i, term->height - 1, 0x20, term->fg, term->bg); }
    term->cx = 0;
    term->cy = term->height - 1;
    term_draw(term);
}

void term_printcaret(terminal_t* term)
{
    term_print_fg(term, "root", COL32_CYAN);
    term_printc(term, '@');
    term_print_fg(term, "A:/", COL32_YELLOW);
    term_print(term, "> ");
}

void term_putc(terminal_t* term, int x, int y, char c, uint32_t fg, uint32_t bg)
{
    if (term == NULL) { return; }
    if ((uint32_t)x >= term->width || (uint32_t)y >= term->height) { return; }
    image_char(&term->buffer, x * font_getw(&FONT_SANS8x14), y * font_geth(&FONT_SANS8x14), c, fg, bg, FONT_SANS8x14);
    term_draw(term);
}

void term_printc(terminal_t* term, char c) { term_printc_fbg(term, c, term->fg, term->bg); }
void term_printc_fg(terminal_t* term, char c, uint32_t fg) { term_printc_fbg(term, c, fg, term->bg); }
void term_printc_fbg(terminal_t* term, char c, uint32_t fg, uint32_t bg)
{
    if (c == '\n') { term_newline(term); }
    else 
    { 
        term_putc(term, term->cx, term->cy, c, fg, bg); 
        term->cx++;
        if (term->cx >= term->width) { term_newline(term); }
    }
}

void term_print(terminal_t* term, char* str) { term_print_fbg(term, str, term->fg, term->bg); }

void term_print_fg(terminal_t* term, char* str, uint32_t fg) { term_print_fbg(term, str, fg, term->bg); }

void term_print_fbg(terminal_t* term, char* str, uint32_t fg, uint32_t bg)
{
    if (str == NULL) { return; }
    int i = 0;
    while (str[i] != 0) { term_printc_fbg(term, str[i], fg, bg); i++; }
}

void term_println(terminal_t* term, char* str) { term_println_fbg(term, str, term->fg, term->bg); }

void term_println_fg(terminal_t* term, char* str, uint32_t fg) { term_println_fbg(term, str, fg, term->bg); }

void term_println_fbg(terminal_t* term, char* str, uint32_t fg, uint32_t bg)
{
    term_print_fbg(term, str, fg, bg);
    term_newline(term);
}

void term_onchar(terminal_t* term, int c)
{
    term_printc(term, c);
}

void term_onenter(terminal_t* term, void* unused)
{
    term_newline(term);
    if (!cmdhandler_execute(term->kb.stream.data, term)) 
    { 
        term_println_fg(term, "Invalid command or file", COL32_TOMATO);
        term_printcaret(term); 
    }
    memset(term->kb.stream.data, 0, term->kb.stream.size);
}

void term_onback(terminal_t* term, void* unused)
{
    term_delete(term);
}

void term_vprintf(terminal_t* term, char* str, va_list args)
{
    while (*str != 0)
    {
        if (*str == '%')
        {
            str++;
            if (*str == '%') { term_printc(term, '%'); }
            if (*str == 'c') { term_printc(term, (char)va_arg(args, int)); }
            else if (*str == 'd' || *str == 'u')
            {
                int dec = va_arg(args, int);
                char str[16];
                memset(str, 0, 16);
                term_print(term, itoa(dec, str, 10));
            }
            else if (*str == 'u')
            {
                uint32_t dec = va_arg(args, uint32_t);
                char str[16];
                memset(str, 0, 16);
                term_print(term, ltoa(dec, str, 10));
            }
            else if (*str == 'x')
            {
                uint32_t dec = va_arg(args, uint32_t);
                char str[16];
                memset(str, 0, 16);
                term_print(term, ltoa(dec, str, 16));
            }
            else if (*str == '2')
            {
                str++;
                if (*str != 'x' && *str != 'X') { term_printc(term, *str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    term_print(term, strhex(num, str, false, 1));
                }
            }
            else if (*str == '4')
            {
                str++;
                if (*str != 'x' && *str != 'X') { term_printc(term, *str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    term_print(term, strhex(num, str, false, 2));
                }
            }
            else if (*str == '8')
            {
                str++;
                if (*str != 'x' && *str != 'X') { term_printc(term, *str); }
                else
                {
                    uint32_t num = va_arg(args, uint32_t);
                    char str[16];
                    memset(str, 0, 16);
                    term_print(term, strhex(num, str, false, 4));
                }
            }
            else if (*str == 'f')
            {
                /* TODO : implement float to string */
            }
            else if (*str == 's')
            {
                char* val = va_arg(args, char*);
                term_print(term, val);
            }
            else { term_printc(term, *str); }
        }
        else { term_printc(term, *str); }
        str++;
    }
}

void term_printf(terminal_t* term, char* str, ...)
{
    va_list args;
    va_start(args, str);
    term_vprintf(term, str, args);
    va_end(args);
}