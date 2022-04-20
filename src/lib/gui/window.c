#include <lib/gui/window.h>
#include <kernel/core/kernel.h>

bool _win_movstart;
int  _win_movx, _win_movy;

void gui_window_btnclose_click(gui_widget_t* widget);
void gui_window_btnmax_click(gui_widget_t* widget);
void gui_window_btnmin_click(gui_widget_t* widget);
void gui_window_movement(gui_window_t* window);

gui_window_t* gui_create_window(int x, int y, int w, int h, char* text)
{
    gui_window_t* window        = gui_create_container(x, y, w, h, text, NULL);
    heapfe(window)->type        = MEMTYPE_WINDOW;
    window->base.base.type      = WIDGET_WINDOW;
    window->state               = WINDOWSTATE_NORMAL;
    window->icon.data           = NULL;
    window->winflags.close_box  = true;
    window->winflags.max_box    = true;
    window->winflags.min_box    = true;
    window->winflags.on_taskbar = true;
    window->winflags.show_icon  = true;
    window->base.base.dispose   = gui_dispose_window;
    window->base.base.update    = gui_update_window;
    window->base.base.draw      = gui_draw_window;
    window->base.base.render    = gui_render_window;

    window->btn_close                   = gui_create_button(0, 0, 16, 15, "", window);
    window->btn_max                     = gui_create_button(0, 0, 16, 15, "", window);
    window->btn_min                     = gui_create_button(0, 0, 16, 15, "", window);
    window->btn_close->base.on_ms_click = gui_window_btnclose_click;
    window->btn_max->base.on_ms_click   = gui_window_btnmax_click;
    window->btn_min->base.on_ms_click   = gui_window_btnmin_click;

    gui_update_window(window);
    gui_draw_window(window);
    gui_render_window(window);
    return window;
}

void gui_dispose_window(gui_window_t* window)
{
    gui_dispose_button(window->btn_close);
    gui_dispose_button(window->btn_max);
    gui_dispose_button(window->btn_min);
    gui_dispose_container(window);
}

void gui_update_window(gui_window_t* window)
{
    gui_update_container(window);
    rect_t tbar = (rect_t){ .x = 1, .y = 1, .width = window->base.base.bounds.width - 3, .height = 19 };

    if (window->base.base.flags.active)
    {
        if (window->btn_close != NULL && window->btn_max != NULL && window->btn_min != NULL) { gui_window_movement(window); }
    }
    else { window->winflags.moving = false; window->winflags.resizing = false; }

    int start_x = tbar.x + (tbar.width - 18);

    if (window->btn_close != NULL && window->winflags.close_box)
    {
        window->btn_close->base.bounds.x = start_x;
        window->btn_close->base.bounds.y = tbar.y + 2;
        window->btn_close->base.update(window->btn_close);
        start_x -= 18;
    }

    if (window->btn_max != NULL && window->winflags.max_box)
    {
        window->btn_max->base.bounds.x   = start_x;
        window->btn_max->base.bounds.y   = tbar.y + 2;
        window->btn_max->base.update(window->btn_max);
        start_x -= 18;
    }

    if (window->btn_min != NULL && window->winflags.min_box)
    {
        window->btn_min->base.bounds.x   = start_x;
        window->btn_min->base.bounds.y   = tbar.y + 2;
        window->btn_min->base.update(window->btn_min);
        start_x -= 18;
    }
}

void gui_draw_window(gui_window_t* window)
{
    gui_draw_container(window);
    rect_t tbar = (rect_t){ .x = 1, .y = 1, .width = window->base.base.bounds.width - 3, .height = 19 };

    uint32_t bg = window->base.base.theme.colors[THEME_TBAR_BG];
    if (!window->base.base.flags.active) { bg = window->base.base.theme.colors[THEME_TBAR_DISABLED]; }
    image_rectf(&window->base.buffer, tbar.x, tbar.y, tbar.width, tbar.height, bg);

    uint32_t iconcol = 0;
    if (window->btn_close != NULL && window->winflags.close_box)
    { 
        iconcol = window->btn_close->base.theme.colors[THEME_FG];
        window->btn_close->base.draw(window->btn_close); 
        image_copy_partialf(&window->base.buffer, window->btn_close->base.bounds.x + 4, window->btn_close->base.bounds.y + 3, ASSETS.tbar.width, 0, 0, 9, 9, COL32_MAGENTA, iconcol, ASSETS.tbar.data);
    }
    if (window->btn_max != NULL  && window->winflags.max_box)
    { 
        window->btn_max->base.draw(window->btn_max); 
        int sx = 9, sy = 0;
        if (window->state == WINDOWSTATE_MAX) { sx = 27; }
        image_copy_partialf(&window->base.buffer, window->btn_max->base.bounds.x + 3, window->btn_max->base.bounds.y + 3, ASSETS.tbar.width, sx, sy, 9, 9, COL32_MAGENTA, iconcol, ASSETS.tbar.data);
    }
    if (window->btn_min != NULL && window->winflags.min_box)
    { 
        window->btn_min->base.draw(window->btn_min); 
        image_copy_partialf(&window->base.buffer, window->btn_min->base.bounds.x + 4, window->btn_min->base.bounds.y + 4, ASSETS.tbar.width, 18, 0, 9, 9, COL32_MAGENTA, iconcol, ASSETS.tbar.data);
    }

    int tx = tbar.x + 2;
    if (window->winflags.show_icon && window->icon.data != NULL)
    {
        image_copy_partial(&window->base.buffer, tbar.x + 2, tbar.y + 2, window->icon.width, window->icon_x, window->icon_y, 16, 16, COL32_MAGENTA, window->icon.data);
        tx += 18;
    }
    
    if (window->base.base.text != NULL) { image_string(&window->base.buffer, tx, tbar.y + 3, window->base.base.text, window->base.base.theme.colors[THEME_TBAR_FG], COL32_TRANSPARENT, *window->base.base.theme.font); }
}

void gui_render_window(gui_window_t* window)
{
    gui_render_container(window);
}

void gui_window_btnclose_click(gui_widget_t* widget)
{
    gui_event_ms_click(widget);    
    gui_window_t* window = widget->parent;
    window->winflags.exited = true;
}

void gui_window_btnmax_click(gui_widget_t* widget)
{
    gui_event_ms_click(widget);
    gui_window_t* window = widget->parent;
    if (window == NULL) { return; }

    if (window->state == WINDOWSTATE_NORMAL)
    {
        window->state = WINDOWSTATE_MAX;
        window->temp_bounds      = window->base.base.bounds;
        window->base.base.bounds = (rect_t){ .x = 0, .y = 0, .width = KARGS.video_mode.width, .height = KARGS.video_mode.height };

    }
    else if (window->state == WINDOWSTATE_MAX)
    {
        window->state            = WINDOWSTATE_NORMAL;
        window->base.base.bounds = window->temp_bounds;
    }
}

void gui_window_btnmin_click(gui_widget_t* widget)
{
    gui_event_ms_click(widget);
}

void gui_window_movement(gui_window_t* window)
{
    if (window->state != WINDOWSTATE_NORMAL) { window->winflags.moving = false; return; }
    rect_t tbar = (rect_t){ .x = window->base.base.bounds.x + 1, .y = window->base.base.bounds.y + 1, .width = window->base.base.bounds.width - 3, .height = 19 };
    bool canmove = (!window->btn_close->base.flags.hover && !window->btn_max->base.flags.hover && !window->btn_min->base.flags.hover);

    if (rect_contains(tbar, mouse_getx(), mouse_gety()))
    {
        if (mouse_btn_left() && canmove)
        {
            if (!_win_movstart)
            {
                _win_movx = mouse_getx() - window->base.base.bounds.x;
                _win_movy = mouse_gety() - window->base.base.bounds.y;
                _win_movstart = true;
            }
            window->winflags.moving = true;
        }
    }

    if (window->winflags.moving)
    {
        window->base.base.bounds.x = mouse_getx() - _win_movx;
        window->base.base.bounds.y = mouse_gety() - _win_movy;
        if (window->base.base.bounds.y < 0) { window->base.base.bounds.y = 0; }
    }

    if (!mouse_btn_left())
    {
        if (window->winflags.moving) { window->winflags.moving = false; }
        _win_movstart = false;
    }
}