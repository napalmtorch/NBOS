#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_listbox_t* gui_create_listbox(int x, int y, int w, int h, gui_container_t* parent);

void gui_dispose_listbox(gui_listbox_t* listbox);
void gui_update_listbox(gui_listbox_t* listbox);
void gui_draw_listbox(gui_listbox_t* listbox);
void gui_render_listbox(gui_listbox_t* listbox);

void listbox_add(gui_listbox_t* listbox, char* value);