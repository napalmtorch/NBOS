#include <lib/gui/tabctrl.h>
#include <kernel/core/kernel.h>

#define TABCTRL_BTNMAX 1024

gui_tabctrl_t* gui_create_tabctrl(int x, int y, int w, int h, gui_container_t* parent)
{
    gui_tabctrl_t* tabctrl = gui_create_widget(x, y, w, h, "tabctrl", parent);
    tabctrl->btns          = tmalloc(TABCTRL_BTNMAX * sizeof(gui_tabbutton_t*), MEMTYPE_PTRARRAY);
    tabctrl->base.dispose  = gui_dispose_tabctrl;
    tabctrl->base.update   = gui_update_tabctrl;
    tabctrl->base.draw     = gui_draw_tabctrl;
    tabctrl->base.render   = gui_render_tabctrl;
    return tabctrl;
}

void gui_dispose_tabctrl(gui_tabctrl_t* tabctrl)
{
    for (int i = 0; i < tabctrl->tabpage_count; i++)
    {
        gui_dispose_button(tabctrl->btns[i]);
        gui_dispose_tabpage(tabctrl->tabpages[i]);
        free(tabctrl->btns[i]);
        free(tabctrl->tabpages[i]);
    }
    free(tabctrl->btns);
    free(tabctrl->tabpages);
    gui_dispose_widget(tabctrl);
}

void gui_update_tabctrl(gui_tabctrl_t* tabctrl)
{
    gui_update_widget(tabctrl);

    int bx = tabctrl->base.bounds.x, by = 0, bw = 88, bh = 20;
    for (uint32_t i = 0; i < tabctrl->tabpage_count; i++)
    {
        if (tabctrl->tabpages[i] == NULL) { continue; }
        tabctrl->tabpages[i]->base.base.bounds = (rect_t){ .x = tabctrl->base.bounds.x, tabctrl->base.bounds.y + 22, tabctrl->base.bounds.width, tabctrl->base.bounds.height - 24 };
        tabctrl->tabpages[i]->base.base.update(tabctrl->tabpages[i]);
        
        by = tabctrl->base.bounds.y + 2, bh = 20;
        if (tabctrl->btns[i] != NULL)
        {
            if (tabctrl->tabpages[i] == tabctrl->selected_tab) 
            { 
                tabctrl->btns[i]->base.base.theme.colors[THEME_BG] = tabctrl->base.theme.colors[THEME_BG]; 
                tabctrl->btns[i]->base.base.theme.colors[THEME_BG_HOVER] = tabctrl->base.theme.colors[THEME_BG_HOVER]; 
                by -= 3;
                bh += 5;
            }
            else 
            {
                tabctrl->btns[i]->base.base.theme.colors[THEME_BG] = tabctrl->base.theme.colors[THEME_BG_HOVER]; 
                tabctrl->btns[i]->base.base.theme.colors[THEME_BG_HOVER] = tabctrl->base.theme.colors[THEME_BG]; 
            }

            bw = font_getsw(tabctrl->btns[i]->base.base.theme.font, tabctrl->btns[i]->base.base.text) + 12;
            tabctrl->btns[i]->base.base.bounds = (rect_t){ .x = bx, .y = by, .width = bw, .height = bh };
            tabctrl->btns[i]->base.base.update(tabctrl->btns[i]);
            bx += tabctrl->btns[i]->base.base.bounds.width;
        }
    }
}

void gui_draw_tabctrl(gui_tabctrl_t* tabctrl)
{
    gui_draw_widget(tabctrl);

    for (uint32_t i = 0; i < tabctrl->tabpage_count; i++)
    {
        if (tabctrl->tabpages[i] == NULL) { continue; }

        if (tabctrl->btns[i] != NULL)
        {
            tabctrl->btns[i]->base.base.draw(tabctrl->btns[i]);
        }
    }

    if (tabctrl->selected_tab != NULL && tabctrl->selected_index >= 0 && tabctrl->selected_index < tabctrl->tabpage_count)
    {
        tabctrl->selected_tab->base.base.draw(tabctrl->selected_tab);
        tabctrl->selected_tab->base.base.render(tabctrl->selected_tab);
    }
}

void gui_render_tabctrl(gui_tabctrl_t* tabctrl)
{
    gui_render_widget(tabctrl);
}

void gui_tabctrl_btndown(gui_widget_t* widget)
{
    gui_event_ms_click(widget);
    gui_tabbutton_t* btn = widget;
    if (btn == NULL || btn->tabctrl == NULL) { return; }
    for (int i = 0; i < TABCTRL_BTNMAX; i++)
    {
        if (btn->tabctrl->btns[i] == NULL) { continue; }
        if (btn->tabctrl->btns[i] == btn) { btn->tabctrl->selected_index = i; btn->tabctrl->selected_tab = btn->tabctrl->tabpages[i]; break; }
    }

    btn->tabctrl->base.update(btn->tabctrl);
    btn->tabctrl->base.draw(btn->tabctrl);
    btn->tabctrl->base.render(btn->tabctrl);
}

void gui_tabctrl_addtab(gui_tabctrl_t* tabctrl, gui_tabpage_t* tab)
{
    gui_tabpage_t** tabs = tmalloc((tabctrl->tabpage_count + 1) * sizeof(gui_tabpage_t*), MEMTYPE_PTRARRAY);

    if (tabctrl->tabpages != NULL)
    {
        memcpy(tabs, tabctrl->tabpages, tabctrl->tabpage_count * sizeof(gui_tabpage_t*));
        free(tabctrl->tabpages);
    }

    gui_tabbutton_t** btns = tmalloc(TABCTRL_BTNMAX * sizeof(gui_tabbutton_t*), MEMTYPE_PTRARRAY);
    memcpy(btns, tabctrl->btns, TABCTRL_BTNMAX * sizeof(gui_tabbutton_t*));
    btns[tabctrl->tabpage_count] = gui_create_button(0, 0, 92, 22, tab->base.base.text, tabctrl->base.parent);
    btns[tabctrl->tabpage_count]->tabctrl = tabctrl;
    btns[tabctrl->tabpage_count]->base.base.on_ms_click = gui_tabctrl_btndown;

    free(tabctrl->btns);
    tabctrl->btns = btns;

    tabctrl->tabpages = tabs;
    tabctrl->tabpages[tabctrl->tabpage_count] = tab;
    tabctrl->tabpage_count++;
    tabctrl->selected_index = tabctrl->tabpage_count - 1;
    tabctrl->selected_tab   = tabctrl->tabpages[tabctrl->selected_index];

    gui_container_t* cont = tabctrl->base.parent;
    while (true) { if (cont == NULL) { break; } cont = cont->base.parent; }
    if (cont != NULL)
    {
        cont->base.update(cont);
        cont->base.draw(cont);
        cont->base.render(cont);
    }

    debug_info("Added tab '%s' to tabctrl '%s'", tab->base.base.text, tabctrl->base.text);
}