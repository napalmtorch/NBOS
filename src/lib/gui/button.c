#include <lib/gui/button.h>
#include <kernel/core/kernel.h>

gui_button_t* gui_create_button(int x, int y, int w, int h, char* text, gui_container_t* parent)
{
    gui_button_t* button = gui_create_widget(x, y, w, h, text, parent);
    button->base.type    = WIDGET_BUTTON;
    button->base.dispose = gui_dispose_button;
    button->base.update  = gui_update_button;
    button->base.draw    = gui_draw_button;
    button->base.render  = gui_render_button;
    return button;
}

void gui_dispose_button(gui_button_t* button)
{
    gui_dispose_widget(button);
}

void gui_update_button(gui_button_t* button)
{
    gui_update_widget(button);
}

void gui_draw_button(gui_button_t* button)
{
    gui_draw_widget(button);

    image_t buff = { 0 };
    if (button->base.parent == NULL) { buff = video_getinfo().buffer; }
    else { buff = button->base.parent->buffer; }
   
    uint32_t bg = button->base.theme.colors[THEME_BG];
    if (button->base.flags.hover && !button->base.flags.down) { bg = button->base.theme.colors[THEME_BG_HOVER]; }

    image_rectf(&buff, button->base.bounds.x, button->base.bounds.y, button->base.bounds.width, button->base.bounds.height, bg);

    if (button->base.theme.border_style == BORDERSTYLE_3D)
    {
        image_rect3d(&buff, button->base.bounds.x, button->base.bounds.y, button->base.bounds.width, button->base.bounds.height, button->base.flags.down, 
                        button->base.theme.colors[THEME_BORDER], button->base.theme.colors[THEME_BORDER_INNER], button->base.theme.colors[THEME_BORDER_OUTER]);
    }

    if (button->base.text != NULL)
    {
        int tw = font_getsw(button->base.theme.font, button->base.text);
        int th = font_geth(button->base.theme.font);
        image_string(&buff, button->base.bounds.x + ((button->base.bounds.width / 2) - (tw / 2)), button->base.bounds.y + ((button->base.bounds.height / 2) - (th / 2)), 
            button->base.text, button->base.theme.colors[THEME_FG], COL32_TRANSPARENT, *button->base.theme.font);
    }
}

void gui_render_button(gui_button_t* button)
{
    gui_render_widget(button);
}