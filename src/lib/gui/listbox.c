#include <lib/gui/listbox.h>
#include <kernel/core/kernel.h>

gui_listbox_t* gui_create_listbox(int x, int y, int w, int h, gui_container_t* parent)
{
    gui_listbox_t* listbox = gui_create_widget(x, y, w, h, "listbox", parent);
    listbox->selected_index = -1;
    listbox->base.type      = WIDGET_LISTBOX;
    listbox->base.theme     = THEME_DEFAULT_LISTBOX;
    listbox->base.dispose   = gui_dispose_listbox;
    listbox->base.update    = gui_update_listbox;
    listbox->base.draw      = gui_draw_listbox;
    listbox->base.render    = gui_render_listbox;
    listbox->scrollbar      = gui_create_scrollbar_y(listbox, parent);
    return listbox;
}

void gui_dispose_listbox(gui_listbox_t* listbox)
{
    freearray(listbox->items, listbox->item_count);
    gui_dispose_widget(listbox);
}

void gui_update_listbox(gui_listbox_t* listbox)
{
    gui_update_widget(listbox);

    if (listbox->selected_index >= listbox->item_count) { listbox->selected_index = listbox->item_count - 1; }

    rect_t bounds = listbox->base.bounds;
    gui_container_t* cont = listbox->base.parent;
    while (cont != NULL) { bounds.x += cont->base.bounds.x; bounds.y += cont->base.bounds.y; cont = cont->base.parent; }

    int sx = 0, sy = 0;
    cont = listbox->base.parent;
    while (cont != NULL) { sx += cont->scroll_x; sy += cont->scroll_y; cont = cont->base.parent; }

    rect_t itembounds = (rect_t) { .x = bounds.x + sx + 2, .y = bounds.y + sy + 2, .width = bounds.width - 3, .height = font_geth(listbox->base.theme.font) };
    for (uint32_t i = 0; i < listbox->item_count; i++)
    {
        if (rect_contains(itembounds, mouse_getx(), mouse_gety()) && mouse_btn_left()) { listbox->selected_index = i; }
        itembounds.y += font_geth(listbox->base.theme.font);
    }

    if (listbox->scrollbar != NULL) 
    { 
        int o1 = (listbox->base.theme.border_style == BORDERSTYLE_3D ? 2 : listbox->base.theme.border_size);
        int o2 = (listbox->base.theme.border_style == BORDERSTYLE_3D ? 1 : listbox->base.theme.border_size);
        listbox->scrollbar->base.bounds.x      = listbox->base.bounds.x + (listbox->base.bounds.width - listbox->scrollbar->base.bounds.width - o2);
        listbox->scrollbar->base.bounds.y      = listbox->base.bounds.y + o1;
        listbox->scrollbar->base.bounds.height = listbox->base.bounds.height;
        listbox->scrollbar->base.update(listbox->scrollbar); 
    }
}

void gui_draw_listbox(gui_listbox_t* listbox)
{
    gui_draw_widget(listbox);

    image_t buff = { 0 };
    if (listbox->base.parent == NULL) { buff = video_getinfo().buffer; }
    else { buff = listbox->base.parent->buffer; }

    int sx = 0, sy = 0;
    gui_container_t* cont = listbox->base.parent;
    while (cont != NULL) { sx += cont->scroll_x; sy += cont->scroll_y; cont = cont->base.parent; }
   
    image_rectf(&buff, listbox->base.bounds.x + sx, listbox->base.bounds.y + sy, listbox->base.bounds.width, listbox->base.bounds.height, listbox->base.theme.colors[THEME_BG]);

    if (listbox->base.theme.border_style == BORDERSTYLE_3D)
    {
        image_rect3d(&buff, listbox->base.bounds.x + sx, listbox->base.bounds.y + sy, listbox->base.bounds.width, listbox->base.bounds.height, true, 
                        listbox->base.theme.colors[THEME_BORDER], listbox->base.theme.colors[THEME_BORDER_INNER], listbox->base.theme.colors[THEME_BORDER_OUTER]);
    }

    rect_t itembounds = (rect_t) { .x = listbox->base.bounds.x + sx + 2, .y = listbox->base.bounds.y + sy + 2, .width = listbox->base.bounds.width - 3, .height = font_geth(listbox->base.theme.font) };
    for (uint32_t i = 0; i < listbox->item_count; i++)
    {
        uint32_t fg = listbox->base.theme.colors[THEME_FG];
        if (i == listbox->selected_index) 
        { 
            image_rectf(&buff, itembounds.x, itembounds.y, itembounds.width, itembounds.height, listbox->base.theme.colors[THEME_TOGGLED]); 
            fg = listbox->base.theme.colors[THEME_FG_HOVER];
        }
        image_string(&buff, itembounds.x + 2, itembounds.y, listbox->items[i], fg, COL32_TRANSPARENT, *listbox->base.theme.font);
        itembounds.y += font_geth(listbox->base.theme.font);
    }

    if (listbox->scrollbar != NULL) { listbox->scrollbar->base.draw(listbox->scrollbar); }
}

void gui_render_listbox(gui_listbox_t* listbox)
{
    gui_render_widget(listbox);
}

void listbox_add(gui_listbox_t* listbox, char* value)
{
    if (listbox == NULL) { return; }

    char** items = tmalloc((listbox->item_count + 1) * sizeof(char*), MEMTYPE_PTRARRAY);

    if (listbox->items != NULL)
    {
        memcpy(items, listbox->items, listbox->item_count * sizeof(char*));
        free(listbox->items);
    }

    listbox->items = items;
    listbox->items[listbox->item_count] = tmalloc(strlen(value) + 1, MEMTYPE_STRING);
    strcpy(listbox->items[listbox->item_count], value);
    listbox->item_count++;
    gui_event_refresh(listbox);
}