#include <lib/gui/widget.h>
#include <kernel/core/kernel.h>

gui_widget_t* gui_create_widget(int x, int y, int w, int h, char* text, gui_container_t* parent)
{
    gui_widget_t* widget  = tmalloc(WIDGET_SIZE, MEMTYPE_GUI);
    widget->bounds        = (rect_t){ .x = x, .y = y, .width = w, .height = h };
    widget->theme         = THEME_DEFAULT;
    widget->parent        = parent;
    widget->type          = WIDGET_BASE;
    widget->flags.active  = true;
    widget->flags.enabled = true;
    widget->settext       = gui_set_text;
    widget->dispose       = gui_dispose_widget;
    widget->update        = gui_update_widget;
    widget->draw          = gui_draw_widget;
    widget->render        = gui_render_widget;
    widget->on_ms_click   = gui_event_ms_click;
    widget->on_ms_down    = gui_event_ms_down;
    widget->on_ms_enter   = gui_event_ms_enter;
    widget->on_ms_leave   = gui_event_ms_leave;
    widget->on_ms_hover   = gui_event_ms_hover;
    gui_set_text(widget, text);
    gui_set_tag(widget, "");
    return widget;
}

void gui_dispose_widget(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    if (widget->text != NULL) { free(widget->text); }
    if (widget->tag  != NULL) { free(widget->tag); }
}

void gui_update_widget(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    rect_t bounds = widget->bounds;

    gui_container_t* cont = widget->parent;
    while (cont != NULL) { bounds.x += cont->base.bounds.x + cont->scroll_x; bounds.y += cont->base.bounds.y + cont->scroll_y; cont = cont->base.parent; }

    if (widget->parent != NULL)
    {
        if (!widget->parent->base.flags.hover) { widget->flags.hover = false; widget->flags.clicked = false; widget->flags.down = false; widget->flags.unclicked = false; return; }
    }

    if (rect_contains(bounds, mouse_getx(), mouse_gety()))
    {
        if (!widget->flags.hover) { widget->flags.hover = true; widget->on_ms_enter(widget); }
        widget->on_ms_hover(widget);

        if (mouse_btn_left()) { widget->flags.down = true; }

        if (mouse_btn_left() && !widget->flags.clicked)
        {
            widget->flags.clicked   = true;
            widget->flags.unclicked = false;
            widget->on_ms_down(widget);
        }

        if (!mouse_btn_left() && !widget->flags.unclicked)
        {
            if (widget->flags.clicked)
            {
                widget->flags.down      = false;
                widget->flags.unclicked = true;
                widget->flags.clicked   = false;
                widget->on_ms_click(widget);
            }
        }

        if (!mouse_btn_left()) { widget->flags.down = false; }
    }
    else
    {
        if (widget->flags.hover) { widget->flags.hover = false; widget->on_ms_leave(widget); }
        widget->flags.down = false;
        widget->flags.clicked = false;
        widget->flags.unclicked = false;
        if (mouse_btn_left() && widget->type != WIDGET_WINDOW) { widget->flags.active = false; }
    }
}

void gui_draw_widget(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    //debug_info("REDRAW - %s", widget->text);
}

void gui_render_widget(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
}

void gui_event_refresh(gui_widget_t* widget)
{
    if (widget->parent == NULL) { widget->draw(widget); widget->render(widget); return; }

    gui_container_t* cont = widget->parent;
    while (true) { if (cont == NULL) { break; } cont->base.draw(cont); cont = cont->base.parent; }
}

void gui_event_ms_click(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    gui_event_refresh(widget);
}

void gui_event_ms_down(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    gui_event_refresh(widget);
}

void gui_event_ms_enter(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    gui_event_refresh(widget);
}

void gui_event_ms_leave(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
    gui_event_refresh(widget);
}

void gui_event_ms_hover(gui_widget_t* widget)
{
    if (widget == NULL) { return; }
}

void gui_set_text(gui_widget_t* widget, char* text)
{
    if (widget == NULL) { return; }
    int len = strlen(text);
    char* str = tmalloc(len + 1, MEMTYPE_STRING);
    if (len > 0) { strcpy(str, text); }
    free(widget->text);
    widget->text = str;
    gui_event_refresh(widget);
}

void gui_set_tag(gui_widget_t* widget, char* tag)
{
    if (widget == NULL) { return; }
    int len = strlen(tag);
    char* str = tmalloc(len + 1, MEMTYPE_STRING);
    if (len > 0) { strcpy(str, tag); }
    free(widget->tag);
    widget->tag = str;
    gui_event_refresh(widget);
}