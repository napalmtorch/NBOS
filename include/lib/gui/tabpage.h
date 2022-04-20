#pragma once
#include <lib/stdint.h>
#include <lib/gui/widget.h>

gui_tabpage_t* gui_create_tabpage(char* text, gui_tabctrl_t* tabctrl);

void gui_dispose_tabpage(gui_tabpage_t* tabpage);
void gui_update_tabpage(gui_tabpage_t* tabpage);
void gui_draw_tabpage(gui_tabpage_t* tabpage);
void gui_render_tabpage(gui_tabpage_t* tabpage);