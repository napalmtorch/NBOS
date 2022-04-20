#include <lib/gui/tabpage.h>
#include <kernel/core/kernel.h>

gui_tabpage_t* gui_create_tabpage(char* text, gui_tabctrl_t* tabctrl)
{
    if (tabctrl == NULL) { kpanic("Tried creating tab page without tab control parent"); return NULL; }
    gui_tabpage_t* tabpage     = gui_create_container(0, 0, 0, 0, text, tabctrl->base.parent);
    tabpage->base.base.parent  = tabctrl->base.parent;
    tabpage->base.base.dispose = gui_dispose_tabpage;
    tabpage->base.base.update  = gui_update_tabpage;
    tabpage->base.base.draw    = gui_draw_tabpage;
    tabpage->base.base.render  = gui_render_tabpage;
    return tabpage;
}

void gui_dispose_tabpage(gui_tabpage_t* tabpage)
{
    gui_dispose_container(tabpage);
}

void gui_update_tabpage(gui_tabpage_t* tabpage)
{
    gui_update_container(tabpage);
}

void gui_draw_tabpage(gui_tabpage_t* tabpage)
{
    gui_draw_container(tabpage);
}

void gui_render_tabpage(gui_tabpage_t* tabpage)
{
    gui_render_container(tabpage);
}