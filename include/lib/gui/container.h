#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_container_t* gui_create_container(int x, int y, int w, int h, char* text, gui_container_t* parent);

void gui_dispose_container(gui_container_t* container);
void gui_update_container(gui_container_t* container);
void gui_draw_container(gui_container_t* container);
void gui_render_container(gui_container_t* container);

void gui_container_add_widget(gui_container_t* container, gui_widget_t* widget);
void gui_container_remove_widget(gui_container_t* container, gui_widget_t* widget);
void gui_container_remove_widget_at(gui_container_t* container, int index);
