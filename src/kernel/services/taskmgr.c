#include <kernel/services/taskmgr.h>
#include <kernel/core/kernel.h>

void taskmgr_btn_threadterm(gui_button_t* button);

taskmgr_t* taskmgr_create(int x, int y)
{
    taskmgr_t* taskmgr = gui_create_window(x, y, 384, 480, "Task Manager");
    taskmgr->window.base.base.update = taskmgr_update;
    taskmgr->window.base.base.draw = taskmgr_draw;
    taskmgr->window.winflags.max_box   = false;
    taskmgr->window.winflags.show_icon = true;
    taskmgr->window.icon   = ASSETS.icons16;
    taskmgr->window.icon_x = 16;
    taskmgr->window.icon_y = 16;  

    taskmgr->tabctrl     = gui_create_tabctrl(8, 32, taskmgr->window.base.base.bounds.width - 16, taskmgr->window.base.base.bounds.height - 40, taskmgr);
    taskmgr->tab_windows = gui_create_tabpage("Windows", taskmgr->tabctrl);
    taskmgr->tab_threads = gui_create_tabpage("Threads", taskmgr->tabctrl);
    taskmgr->tab_stats   = gui_create_tabpage("Stats", taskmgr->tabctrl);
    taskmgr->tab_startup = gui_create_tabpage("Startup", taskmgr->tabctrl);
    gui_tabctrl_addtab(taskmgr->tabctrl, taskmgr->tab_windows);
    gui_tabctrl_addtab(taskmgr->tabctrl, taskmgr->tab_threads);
    gui_tabctrl_addtab(taskmgr->tabctrl, taskmgr->tab_stats);
    gui_tabctrl_addtab(taskmgr->tabctrl, taskmgr->tab_startup);
    gui_container_add_widget(taskmgr, taskmgr->tabctrl);

    taskmgr->tab_startup->base.scroll_x += 300;
    taskmgr->tab_startup->base.scroll_y += 50;
    gui_container_add_widget(taskmgr->tab_startup, gui_create_button(32, 32, 92, 22, "TestBtn", taskmgr->tab_startup));

    taskmgr->list_windows = gui_create_listbox(8, 8, taskmgr->tab_windows->base.base.bounds.width - 16, taskmgr->tab_windows->base.base.bounds.height - 16, taskmgr->tab_windows);
    gui_container_add_widget(taskmgr->tab_windows, taskmgr->list_windows);

    taskmgr->list_threads = gui_create_listbox(8, 28, taskmgr->tab_threads->base.base.bounds.width - 16, taskmgr->tab_threads->base.base.bounds.height - 72, taskmgr->tab_threads);
    gui_container_add_widget(taskmgr->tab_threads, taskmgr->list_threads);

    taskmgr->label_memused = gui_create_label(8, 8, "Heap Used: ", taskmgr->tab_stats);
    gui_container_add_widget(taskmgr->tab_stats, taskmgr->label_memused);

    taskmgr->label_memtotal = gui_create_label(8, 50, "Heap Total: ", taskmgr->tab_stats);
    gui_container_add_widget(taskmgr->tab_stats, taskmgr->label_memtotal);

    taskmgr->label_cpuusage = gui_create_label(8, 92, "CPU Usage:  0%", taskmgr->tab_stats);
    gui_container_add_widget(taskmgr->tab_stats, taskmgr->label_cpuusage);

    taskmgr->label_threadrows = gui_create_label(8, 4, "NAME            MEM USAGE    CPU USAGE", taskmgr->tab_threads);
    gui_container_add_widget(taskmgr->tab_threads, taskmgr->label_threadrows);

    taskmgr->btn_thread_term = gui_create_button(taskmgr->list_threads->base.bounds.x + taskmgr->list_threads->base.bounds.width - 92, 
                                                 taskmgr->list_threads->base.bounds.y + taskmgr->list_threads->base.bounds.height + 8, 92, 22, "Terminate", taskmgr->tab_threads);
    taskmgr->btn_thread_term->base.on_ms_click = taskmgr_btn_threadterm;
    gui_container_add_widget(taskmgr->tab_threads, taskmgr->btn_thread_term);
    free(taskmgr->btn_thread_term->base.tag);

    taskmgr->tabctrl->selected_tab   = taskmgr->tab_threads;
    taskmgr->tabctrl->selected_index = 1;
    return taskmgr;
}

void taskmgr_update(taskmgr_t* taskmgr)
{
    taskmgr->time = pit_get_seconds();
    if (taskmgr->time != taskmgr->timelast)
    {
        taskmgr->timelast = taskmgr->time;
        taskmgr_update_strings(taskmgr);

        freearray(taskmgr->list_windows->items, taskmgr->list_windows->item_count);
        taskmgr->list_windows->items      = NULL;
        taskmgr->list_windows->item_count = 0;

        gui_window_t** windows = winmgr_get_list();
        for (uint32_t i = 0; i < winmgr_get_count(); i++)
        {
            listbox_add(taskmgr->list_windows, windows[i]->base.base.text);
        }

        freearray(taskmgr->list_threads->items, taskmgr->list_threads->item_count);
        taskmgr->list_threads->items      = NULL;
        taskmgr->list_threads->item_count = 0;

        taskmgr->btn_thread_term->base.tag = taskmgr->list_threads;

        for (uint32_t i = 0; i < THREADMGR.count; i++)
        {
            char txt[160];
            memset(txt, 0, 160);
            strcat(txt, THREADMGR.threads[i]->name);
            while (strlen(txt) < 16) { stradd(txt, ' '); }
            char temp[32];
            memset(temp, 0, 32);
            strcat(txt, ltoa((heap_calc_used(&HEAP_LARGE, THREADMGR.threads[i]) + heap_calc_used(&HEAP_SMALL, THREADMGR.threads[i])) / 1024, temp, 10));
            strcat(txt, " KB");

            memset(temp, 0, 32);
            while (strlen(txt) < 29) { stradd(txt, ' '); }
            strcat(txt, ltoa(THREADMGR.threads[i]->cpu_usage, temp, 10));
            stradd(txt, '%');

            listbox_add(taskmgr->list_threads, txt);
        }        
    }

    gui_update_window(taskmgr);
}

void taskmgr_draw(taskmgr_t* taskmgr)
{
    gui_draw_window(taskmgr);
}

void taskmgr_update_strings(taskmgr_t* taskmgr)
{
    char temp1[64];
    char temp2[64];
    
    uint32_t mem_used = heap_calc_used(&HEAP_SMALL, NULL) + heap_calc_used(&HEAP_LARGE, NULL);
    uint32_t mem_total = HEAP_SMALL.size + HEAP_LARGE.size;
    
    // heap used label
    memset(temp1, 0, 64);
    memset(temp2, 0, 64);
    strcat(temp1, "Heap Used:  ");
    strcat(temp1, ltoa(mem_used / 1024 / 1024, temp2, 10));
    strcat(temp1, " MB\n"); 
    memset(temp2, 0, 64);
    strcat(temp1, "            ");
    strcat(temp1, ltoa(mem_used, temp2, 10));
    strcat(temp1, " bytes");
    taskmgr->label_memused->base.settext(taskmgr->label_memused, temp1);

    // heap total label
    memset(temp1, 0, 64);
    memset(temp2, 0, 64);
    strcat(temp1, "Heap Total: ");
    strcat(temp1, ltoa(mem_total / 1024 / 1024, temp2, 10));
    strcat(temp1, " MB\n"); 
    memset(temp2, 0, 64);
    strcat(temp1, "            ");
    strcat(temp1, ltoa(mem_total, temp2, 10));
    strcat(temp1, " bytes");
    taskmgr->label_memtotal->base.settext(taskmgr->label_memtotal, temp1);

    memset(temp1, 0, 64);
    memset(temp2, 0, 64);
    strcat(temp1, "CPU Usage:  ");
    strcat(temp1, ltoa(taskmgr_calc_usage(), temp2, 10));
    strcat(temp1, "%");
    taskmgr->label_cpuusage->base.settext(taskmgr->label_cpuusage, temp1);
}

uint32_t taskmgr_calc_usage()
{
    uint32_t total_ticks = 0, idle_ticks;
    for (int i = 0; i < THREADMGR.count; i++)
    {
        if (!strcmp(THREADMGR.threads[i]->name, "kernel")) { idle_ticks = THREADMGR.threads[i]->ptps; }
        total_ticks += THREADMGR.threads[i]->ptps;
    }

    uint32_t usage = 0;
    for (int i = 0; i < THREADMGR.count; i++)
    {
        double num = (double)THREADMGR.threads[i]->ptps / (double)total_ticks;
        THREADMGR.threads[i]->cpu_usage = (uint32_t)((double)num * 100.0);
        if (strcmp(THREADMGR.threads[i]->name, "kernel")) { usage += THREADMGR.threads[i]->cpu_usage; }
    }
    return usage;
}

void taskmgr_btn_threadterm(gui_button_t* button)
{
    gui_event_ms_click(button);
    gui_listbox_t* threads = button->base.tag;
    if (threads == NULL) { return; }
    if (threads->selected_index < 0 || threads->selected_index >+ threads->item_count) { return; }

    char* sel = threads->items[threads->selected_index];
    char tname[64];
    memset(tname, 0, 64);

    int i = 0;
    for (int i = 0; i < strlen(sel); i++)
    {
        if (sel[i] == ' ') { break; }
        else { stradd(tname, sel[i]); }
    }

    for (i = 0; i < THREADMGR.count; i++)
    {
        if (!strcmp(tname, THREADMGR.threads[i]->name))
        {
            threadmgr_terminate(THREADMGR.threads[i]);
            return;
        }
    }
}