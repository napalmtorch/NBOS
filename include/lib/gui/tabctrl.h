#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_tabctrl_t* gui_create_tabctrl(int x, int y, int w, int h, gui_container_t* parent);

void gui_dispose_tabctrl(gui_tabctrl_t* tabctrl);
void gui_update_tabctrl(gui_tabctrl_t* tabctrl);
void gui_draw_tabctrl(gui_tabctrl_t* tabctrl);
void gui_render_tabctrl(gui_tabctrl_t* tabctrl);

void gui_tabctrl_addtab(gui_tabctrl_t* tabctrl, gui_tabpage_t* tab);