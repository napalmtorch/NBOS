#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_window_t* gui_create_window(int x, int y, int w, int h, char* text);

void gui_dispose_window(gui_window_t* window);
void gui_update_window(gui_window_t* window);
void gui_draw_window(gui_window_t* window);
void gui_render_window(gui_window_t* window);
