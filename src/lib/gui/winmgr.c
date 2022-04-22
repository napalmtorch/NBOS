#include <lib/gui/winmgr.h>
#include <kernel/core/kernel.h>

gui_window_t** _win_list;
gui_window_t*  _win_active;
gui_window_t*  _win_maxed;
uint32_t       _win_count;
int32_t        _win_index;
bool           _win_msdown;
int            _win_top;

void winmgr_init()
{   
    _win_count  = 0;
    _win_index  = -1;
    _win_top    = -1;
    _win_active = NULL;
    _win_maxed  = false;
    _win_msdown = false;
    _win_list   = NULL;
    debug_ok("Initialized window manager");
}

void winmgr_update()
{
    int highest_hover = -1;
    int hover = 0, moving = 0, resize = 0, i = 0;
    int moving_i = -1;

    for (i = 0; i < _win_count; i++)
    {
        if (_win_list[i] == NULL) { continue; }

        if (_win_list[i]->winflags.exited) { winmgr_unload(_win_list[i], true); i = -1; }

        if (_win_list[i]->base.base.flags.hover) { hover++; }
        if (_win_list[i]->winflags.moving) { moving++; }
        if (_win_list[i]->winflags.resizing) { resize++; }
    }

    for (i = _win_count - 1; i >= 0; i--)
    {
        if (_win_list[i] == NULL) { continue; }
        _win_list[i]->base.base.update(_win_list[i]);
        if (moving + resize > 0) { continue; }

        if (_win_active != NULL)
        {
            if (_win_list[i]->base.base.flags.hover && !_win_active->base.base.flags.hover && _win_list[i] != _win_active && mouse_btn_left() && !_win_msdown)
            {
                if (i > highest_hover) { highest_hover = i; _win_top = i; } 
                _win_msdown = true;
            }
        }
        else
        {
            if (_win_list[i]->base.base.flags.hover && mouse_btn_left() && !_win_msdown)
            {
                if (i > highest_hover) { highest_hover = i; _win_top = i; } 
                _win_msdown = true;   
            }
        }
    }

    if (hover == 0 && moving == 0 && resize == 0 && mouse_btn_left() && !_win_msdown) { winmgr_set_active(NULL); _win_msdown = true; }

    if (highest_hover >= 0) { winmgr_set_active(_win_list[highest_hover]); }

    if (!mouse_btn_left()) { _win_msdown = false; }
}

void winmgr_render()
{
    for (uint32_t i = 0; i < _win_count; i++)
    {
        if (_win_list[i] == NULL) { continue; }
        _win_list[i]->base.base.render(_win_list[i]);
    }
}

void winmgr_force_refresh()
{
     for (uint32_t i = 0; i < _win_count; i++)
    {
        if (_win_list[i] == NULL) { continue; }
        if (_win_list[i] == _win_active) { _win_list[i]->base.base.flags.active = true; }
        else { _win_list[i]->base.base.flags.active = false; }
        _win_list[i]->base.base.update(_win_list[i]);
        _win_list[i]->base.base.draw(_win_list[i]);
        _win_list[i]->base.base.render(_win_list[i]);
    }
}

void winmgr_load(gui_window_t* window)
{
    if (window == NULL) { return; }

    gui_window_t** windows = tmalloc((_win_count + 1) * sizeof(gui_window_t*), MEMTYPE_PTRARRAY);

    if (_win_list != NULL)
    {
        memcpy(windows, _win_list, _win_count * sizeof(gui_window_t*));
        free(_win_list);
    }

    _win_list = windows;
    _win_list[_win_count] = window;
    _win_active = window;
    _win_index = _win_count;
    _win_count++;
    winmgr_force_refresh();
    debug_ok("Loaded window - '%s'", window->base.base.text);
}

bool winmgr_unload(gui_window_t* window, bool dispose)
{
    if (window == NULL) { return false; }

    for (uint32_t i = 0; i < _win_count; i++)
    {
        if (_win_list[i] == NULL) { continue; }
        if (_win_list[i] == window)
        {
            if (_win_count == 1) 
            { 
                debug_info("Unloaded window - '%s'", window->base.base.text);
                if (dispose) { gui_dispose_window(window); }
                _win_count = 0; free(_win_list); _win_list = NULL; _win_index = -1; _win_active = NULL; 
                winmgr_force_refresh();
                return true;
            }

            gui_window_t** windows = tmalloc((_win_count - 1) * sizeof(gui_window_t*), MEMTYPE_PTRARRAY);
            uint32_t pos = 0;

            for (uint32_t j = 0; j < _win_count; j++)
            {
                if (_win_list[j] != window) { windows[pos] = _win_list[j]; pos++; }
            }
            free(_win_list);
            _win_count--;
            _win_list = windows;
            _win_active = _win_list[_win_count - 1];
            _win_index = _win_count - 1;
            window->base.base.flags.active = true;
            debug_info("Unloaded window - '%s'", window->base.base.text);
            if (dispose) { gui_dispose_window(window); }
            winmgr_force_refresh();
            return true;
        }
    }

    kpanic("Failed to unload window - '%s'", window->base.base.text);
    return false;
}

gui_window_t* winmgr_set_active(gui_window_t* window)
{
    if (window == NULL)
    {
        _win_active = NULL;
        _win_index = -1;
        winmgr_force_refresh();
        debug_info("Set active window to NULL");
        return NULL;
    }

    winmgr_unload(window, false);
    winmgr_load(window);
    window->base.base.flags.active = true;
    winmgr_force_refresh();

    debug_info("Set active window to %s", window->base.base.text);
    return window;
}

gui_window_t** winmgr_get_list() { return _win_list; }

gui_window_t* winmgr_get_active() { return _win_active; }

uint32_t winmgr_get_count() { return _win_count; }