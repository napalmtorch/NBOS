#include <kernel/services/terminal.h>
#include <kernel/core/kernel.h>

terminal_t* term_create(int x, int y)
{
    terminal_t* term                = gui_create_window(x, y, 640, 400, "Terminal");
    term->window.base.base.update   = term_update;
    term->window.base.base.draw     = term_draw;
    term->window.base.base.render   = term_render;
    term->window.winflags.show_icon = true;
    term->window.icon               = ASSETS.icons16;
    term->window.icon_x             = 64;
    term->window.icon_y             = 48;
    
    term->width                     = (640 - (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 3 : 2)) / font_getw(&FONT_SANS8x14);
    term->height                    = (400 - (term->window.base.base.theme.border_style == BORDERSTYLE_3D ? 3 : 2) - 19) / font_geth(&FONT_SANS8x14);
    term->buffer                    = image_create(term->width * font_getw(&FONT_SANS8x14), term->height * font_geth(&FONT_SANS8x14));
    term->fg                        = COL32_WHITE;
    term->bg                        = COL32_BLACK;
    term_clear(term);
    term_println_fg(term, "Napalmtorch Terminal", COL32_CYAN);
    term_println(term, "Version 1.0");
    term_println_fg(term, "All rights reserved", COL32_GRAY);
    return term;
}

void term_update(terminal_t* term)
{
    gui_update_window(term);
    term->window.base.base.theme.colors[THEME_BG] = term->bg;
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
}

void term_scroll(terminal_t* term)
{
    if (term == NULL) { return; }
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