#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <lib/gui/widget.h>

extern int _win_top;

void winmgr_init();
void winmgr_update();
void winmgr_render();
void winmgr_force_refresh();
void winmgr_load(gui_window_t* window);
bool winmgr_unload(gui_window_t* window, bool dispose);
gui_window_t* winmgr_set_active(gui_window_t* window);

gui_window_t** winmgr_get_list();
gui_window_t* winmgr_get_active();
uint32_t winmgr_get_count();
