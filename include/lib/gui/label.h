#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_label_t* gui_create_label(int x, int y, char* text, gui_container_t* parent);

void gui_dispose_label(gui_label_t* label);
void gui_update_label(gui_label_t* label);
void gui_draw_label(gui_label_t* label);
void gui_render_label(gui_label_t* label);