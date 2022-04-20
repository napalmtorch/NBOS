#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_button_t* gui_create_button(int x, int y, int w, int h, char* text, gui_container_t* parent);

void gui_dispose_button(gui_button_t* button);
void gui_update_button(gui_button_t* button);
void gui_draw_button(gui_button_t* button);
void gui_render_button(gui_button_t* button);