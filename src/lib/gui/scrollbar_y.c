#include <lib/gui/scrollbar_y.h>
#include <kernel/core/kernel.h>

gui_scrollbar_y_t* gui_create_scrollbar_y(gui_widget_t* widget, gui_container_t* parent)
{
    gui_scrollbar_y_t* bar = gui_create_widget(0, 0, 18, 200, "scrollbar_y", parent);
    bar->base.type    = WIDGET_SCROLLBAR;
    bar->base.dispose = gui_dispose_scrollbar_y;
    bar->base.update  = gui_update_scrollbar_y;
    bar->base.draw    = gui_draw_scrollbar_y;
    bar->base.render  = gui_render_scrollbar_y;
    bar->btn_up       = gui_create_button(0, 0, 18, 18, "U", parent);
    bar->btn_down     = gui_create_button(0, 0, 18, 18, "D", parent);
    bar->widget = widget;
    return bar;
}

void gui_dispose_scrollbar_y(gui_scrollbar_y_t* scrollbar_y)
{
    gui_dispose_widget(scrollbar_y);
}

void gui_update_scrollbar_y(gui_scrollbar_y_t* scrollbar_y)
{
    gui_update_widget(scrollbar_y);

    if (scrollbar_y->btn_up != NULL) 
    { 
        scrollbar_y->btn_up->base.bounds.x = scrollbar_y->base.bounds.x;
        scrollbar_y->btn_up->base.bounds.y = scrollbar_y->base.bounds.y;
        scrollbar_y->btn_up->base.update(scrollbar_y->btn_up); 
    }

    if (scrollbar_y->btn_down != NULL) 
    { 
        scrollbar_y->btn_down->base.bounds.x = scrollbar_y->base.bounds.x;
        scrollbar_y->btn_down->base.bounds.y = scrollbar_y->base.bounds.height - scrollbar_y->btn_down->base.bounds.height;
        scrollbar_y->btn_down->base.update(scrollbar_y->btn_down); 
    }
}

void gui_draw_scrollbar_y(gui_scrollbar_y_t* scrollbar_y)
{
    gui_draw_widget(scrollbar_y);
    
    if (scrollbar_y->btn_up != NULL)   { scrollbar_y->btn_up->base.draw(scrollbar_y->btn_up); }
    if (scrollbar_y->btn_down != NULL) { scrollbar_y->btn_down->base.draw(scrollbar_y->btn_down); }
}

void gui_render_scrollbar_y(gui_scrollbar_y_t* scrollbar_y)
{
    gui_render_widget(scrollbar_y);
}