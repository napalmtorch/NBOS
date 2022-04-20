#pragma once
#include <lib/stdint.h>
#include <lib/gui/window.h>
#include <lib/gui/widget.h>

typedef struct
{
    gui_window_t   window;
    uint32_t       time, timelast;

    gui_tabctrl_t* tabctrl;
    gui_tabpage_t* tab_windows;
    gui_tabpage_t* tab_threads;
    gui_tabpage_t* tab_stats;
    gui_tabpage_t* tab_startup;

    gui_label_t*   label_memused;
    gui_label_t*   label_memtotal;
    gui_label_t*   label_cpuusage;

    gui_listbox_t* list_windows;
    gui_listbox_t* list_threads;

} PACKED taskmgr_t;

taskmgr_t* taskmgr_create(int x, int y);

void taskmgr_update(taskmgr_t* taskmgr);
void taskmgr_draw(taskmgr_t* taskmgr);
void taskmgr_update_strings(taskmgr_t* taskmgr);
uint32_t taskmgr_calc_usage();