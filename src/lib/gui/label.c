#include <lib/gui/label.h>
#include <kernel/core/kernel.h>

gui_label_t* gui_create_label(int x, int y, char* text, gui_container_t* parent)
{
    gui_label_t* label = gui_create_widget(x, y, 1, 1, text, parent);
    label->base.type    = WIDGET_LABEL;
    label->base.theme   = THEME_DEFAULT_LABEL;
    label->base.dispose = gui_dispose_label;
    label->base.update  = gui_update_label;
    label->base.draw    = gui_draw_label;
    label->base.render  = gui_render_label;
    label->autosize     = true;
    return label;
}

void gui_dispose_label(gui_label_t* label)
{
    gui_dispose_widget(label);
}

void gui_update_label(gui_label_t* label)
{
    gui_update_widget(label);
}

void gui_draw_label(gui_label_t* label)
{
    gui_draw_widget(label);

    image_t buff = { 0 };
    if (label->base.parent == NULL) { buff = video_getinfo().buffer; }
    else { buff = label->base.parent->buffer; }

    int sx = 0, sy = 0;
    gui_container_t* cont = label->base.parent;
    while (cont != NULL) { sx += cont->scroll_x; sy += cont->scroll_y; cont = cont->base.parent; }


    image_rectf(&buff, label->base.bounds.x + sx, label->base.bounds.y + sy, label->base.bounds.width, label->base.bounds.height, label->base.theme.colors[THEME_BG]);

    if (label->base.theme.border_style == BORDERSTYLE_3D)
    {
        image_rect3d(&buff, label->base.bounds.x + sx, label->base.bounds.y + sy, label->base.bounds.width, label->base.bounds.height, label->base.flags.down, 
                        label->base.theme.colors[THEME_BORDER], label->base.theme.colors[THEME_BORDER_INNER], label->base.theme.colors[THEME_BORDER_OUTER]);
    }

    if (label->base.text != NULL)
    {
        int tw = font_getsw(label->base.theme.font, label->base.text);
        int th = font_geth(label->base.theme.font);
        if (label->autosize) 
        { 
            label->base.bounds.width = tw + 8; label->base.bounds.height = th + 8; 
            image_string(&buff, label->base.bounds.x + 4 + sx, label->base.bounds.y + 4 + sy, label->base.text, label->base.theme.colors[THEME_FG], COL32_TRANSPARENT, *label->base.theme.font);
        }
        else
        {
            image_string(&buff, label->base.bounds.x + sx + ((label->base.bounds.width / 2) - (tw / 2)), label->base.bounds.y + sy + ((label->base.bounds.height / 2) - (th / 2)), 
            label->base.text, label->base.theme.colors[THEME_FG], COL32_TRANSPARENT, *label->base.theme.font);
        }
    }
    else
    {
        if (label->autosize) { label->base.bounds.width = 1; label->base.bounds.height = 1; }
    }
}

void gui_render_label(gui_label_t* label)
{
    gui_render_widget(label);
}