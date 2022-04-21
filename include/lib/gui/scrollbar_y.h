#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_scrollbar_y_t* gui_create_scrollbar_y(gui_widget_t* widget, gui_container_t* parent);

void gui_dispose_scrollbar_y(gui_scrollbar_y_t* scrollbar_y);
void gui_update_scrollbar_y(gui_scrollbar_y_t* scrollbar_y);
void gui_draw_scrollbar_y(gui_scrollbar_y_t* scrollbar_y);
void gui_render_scrollbar_y(gui_scrollbar_y_t* scrollbar_y);