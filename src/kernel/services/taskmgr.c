#include <kernel/services/taskmgr.h>
#include <kernel/core/kernel.h>

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

    taskmgr->list_threads = gui_create_listbox(8, 8, taskmgr->tab_threads->base.base.bounds.width - 16, taskmgr->tab_threads->base.base.bounds.height - 16, taskmgr->tab_threads);
    gui_container_add_widget(taskmgr->tab_threads, taskmgr->list_threads);

    taskmgr->label_memused = gui_create_label(8, 8, "Heap Used: ", taskmgr->tab_stats);
    gui_container_add_widget(taskmgr->tab_stats, taskmgr->label_memused);

    taskmgr->label_memtotal = gui_create_label(8, 50, "Heap Total: ", taskmgr->tab_stats);
    gui_container_add_widget(taskmgr->tab_stats, taskmgr->label_memtotal);

    taskmgr->label_cpuusage = gui_create_label(8, 92, "CPU Usage:  0%", taskmgr->tab_stats);
    gui_container_add_widget(taskmgr->tab_stats, taskmgr->label_cpuusage);

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

        for (uint32_t i = 0; i < THREADMGR.count; i++)
        {
            char txt[160];
            memset(txt, 0, 160);
            strcat(txt, THREADMGR.threads[i]->name);
            while (strlen(txt) < 24) { stradd(txt, ' '); }
            char temp[32];
            memset(temp, 0, 32);
            strcat(txt, ltoa((heap_calc_used(&HEAP_LARGE, THREADMGR.threads[i]) + heap_calc_used(&HEAP_SMALL, THREADMGR.threads[i])) / 1024, temp, 10));
            strcat(txt, " KB");
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
        if (!strcmp(THREADMGR.threads[i]->name, "idle")) { idle_ticks = THREADMGR.threads[i]->tps; }
        total_ticks += THREADMGR.threads[i]->tps;
    }
    if (idle_ticks >= 100) { idle_ticks /= 100; total_ticks /= 100; }
    else if (idle_ticks >= 1000) { idle_ticks /= 1000; total_ticks /= 1000; }

    uint32_t tmi = total_ticks - idle_ticks;

    double usage = (double)tmi / (double)total_ticks;
    usage *= 100.0;

    return usage;
}