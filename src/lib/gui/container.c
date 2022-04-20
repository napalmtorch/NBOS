#include <lib/gui/container.h>
#include <kernel/core/kernel.h>

gui_container_t* gui_create_container(int x, int y, int w, int h, char* text, gui_container_t* parent)
{
    gui_container_t* container = gui_create_widget(x, y, w, h, text, NULL);
    container->base.type = WIDGET_CONTAINER;
    container->buffer = image_create(w, h);
    container->old_w = w;
    container->old_h = h;
    container->widget_count = 0;
    container->widgets = NULL;
    container->client_bounds = (rect_t){ .x = x + 1, .y = y + 21, .width = w - 3, .height = h - 24 };
    container->base.parent  = parent;
    container->base.dispose = gui_dispose_container;
    container->base.update  = gui_update_container;
    container->base.draw    = gui_draw_container;
    container->base.render  = gui_render_container;

    gui_update_container(container);
    gui_draw_container(container);
    gui_render_container(container);
    return container;
}

void gui_dispose_container(gui_container_t* container)
{
    if (container->widgets != NULL)
    {
        for (int i = 0; i < container->widget_count; i++) { container->widgets[i]->dispose(container->widgets[i]); free(container->widgets[i]); }
        free(container->widgets);
    }
    image_dispose(&container->buffer);   
    gui_dispose_widget(container);
}

void gui_update_container(gui_container_t* container)
{
    gui_update_widget(container);

    if (container->base.bounds.width != container->old_w || container->base.bounds.height != container->old_h)
    {
        container->old_w = container->base.bounds.width;
        container->old_h = container->base.bounds.height;
        image_dispose(&container->buffer);
        container->buffer = image_create(container->base.bounds.width, container->base.bounds.height);
        container->base.update(container);
        container->base.draw(container);
        container->base.render(container);
    }

    for (uint32_t i = 0; i < container->widget_count; i++)
    {
        if (container->widgets[i] == NULL) { continue; }
        container->widgets[i]->update(container->widgets[i]);
    }
}

void gui_draw_container(gui_container_t* container)
{
    gui_draw_widget(container);

    image_clear(&container->buffer, container->base.theme.colors[THEME_BG]);

    if (container->base.theme.border_style == BORDERSTYLE_3D)
    {
        image_rect3d(&container->buffer, 0, 0, container->base.bounds.width, container->base.bounds.height, false, 
                        container->base.theme.colors[THEME_BORDER], container->base.theme.colors[THEME_BORDER_INNER], container->base.theme.colors[THEME_BORDER_OUTER]);
    }

    for (uint32_t i = 0; i < container->widget_count; i++)
    {
        if (container->widgets[i] == NULL) { continue; }
        container->widgets[i]->draw(container->widgets[i]);
        if (container->widgets[i]->type == WIDGET_CONTAINER && container->widgets[i]->parent != NULL)
        {
            container->widgets[i]->render(container->widgets[i]);    
        }
    }
}

void gui_render_container(gui_container_t* container)
{
    gui_render_widget(container);
    image_t vbuff = video_getinfo().buffer;
    if (container->base.parent != NULL) { vbuff = container->base.parent->buffer; }
    image_copy(&vbuff, container->base.bounds.x, container->base.bounds.y, container->buffer.width, container->buffer.height, container->buffer.data);
}

void gui_container_add_widget(gui_container_t* container, gui_widget_t* widget)
{
    gui_widget_t** widgets = tmalloc((container->widget_count + 1) * sizeof(gui_widget_t*), MEMTYPE_PTRARRAY);

    if (container->widgets != NULL)
    {
        memcpy(widgets, container->widgets, container->widget_count * sizeof(gui_widget_t*));
        free(container->widgets);
    }

    container->widgets = widgets;
    container->widgets[container->widget_count] = widget;
    container->widget_count++;

    container->base.update(container);
    container->base.draw(container);
    container->base.render(container);

    debug_info("Added widget '%s' to container '%s'", widget->text, container->base.text);
}

void gui_container_remove_widget(gui_container_t* container, gui_widget_t* widget)
{
    
}

void gui_container_remove_widget_at(gui_container_t* container, int index)
{

}