#pragma once
#include <lib/stdint.h>
#include <lib/graphics/image.h>
#include <lib/graphics/color.h>
#include <lib/graphics/primitives.h>
#include <lib/gui/theme.h>

#define WIDGET_SIZE 4096

typedef enum
{
    WIDGET_BASE,
    WIDGET_CONTAINER,
    WIDGET_WINDOW,
    WIDGET_BUTTON,
    WIDGET_TEXTBOX,
    WIDGET_LABEL,
    WIDGET_TABPAGE,
    WIDGET_TABCTRL,
    WIDGET_LISTBOX,
} WIDGET;

typedef enum
{
    WINDOWSTATE_NORMAL,
    WINDOWSTATE_MAX,
    WINDOWSTATE_MIN,
} WINDOWSTATE;

typedef struct
{
    bool close_box : 1;
    bool max_box    : 1;
    bool min_box    : 1;
    bool moving     : 1;
    bool resizing   : 1;
    bool exited     : 1;
    bool on_taskbar : 1;
    bool show_icon  : 1;
} PACKED gui_windowflags_t;

typedef struct
{
    bool hover, down, clicked, unclicked;
    bool active, enabled, refreshed, toggled;
} PACKED gui_flags_t;

typedef struct gui_widget_t gui_widget_t;
typedef struct gui_container_t gui_container_t;
typedef struct gui_window_t gui_window_t;
typedef struct gui_button_t gui_button_t;
typedef struct gui_tabpage_t gui_tabpage_t;
typedef struct gui_tabctrl_t gui_tabctrl_t;
typedef struct gui_tabbutton_t gui_tabbutton_t;
typedef struct gui_label_t gui_label_t;
typedef struct gui_listbox_t gui_listbox_t;

typedef void (*gui_event_t)(gui_widget_t* this);
typedef void (*gui_strevent_t)(gui_widget_t* this, char* str);

struct gui_widget_t
{
    gui_container_t* parent;
    rect_t           bounds;
    gui_flags_t      flags;
    gui_theme_t      theme;
    WIDGET           type;
    char*            text;
    char*            tag;

    gui_event_t   dispose;
    gui_event_t   update;
    gui_event_t   draw;
    gui_event_t   render;
    gui_event_t   on_ms_click;
    gui_event_t   on_ms_down;
    gui_event_t   on_ms_enter;
    gui_event_t   on_ms_leave;
    gui_event_t   on_ms_hover;
    gui_strevent_t   settext;
};

struct gui_container_t
{
    gui_widget_t   base;
    gui_widget_t** widgets;
    uint32_t       widget_count;
    rect_t         client_bounds;
    image_t        buffer;
    int            old_w, old_h;
    int            scroll_x, scroll_y;
};

struct gui_window_t
{
    gui_container_t   base;
    gui_button_t*     btn_close;
    gui_button_t*     btn_max;
    gui_button_t*     btn_min;
    gui_windowflags_t winflags;
    WINDOWSTATE       state;
    rect_t            temp_bounds;
    image_t           icon;
    int               icon_x, icon_y;
};

struct gui_label_t
{
    gui_widget_t base;
    bool         autosize;
};

struct gui_listbox_t
{
    gui_widget_t base;
    char**       items;
    uint32_t     item_count;
    int          selected_index;
};

struct gui_button_t
{
    gui_widget_t base;
    bool         can_toggle : 1;
};

struct gui_tabbutton_t
{
    gui_button_t     base;
    gui_tabctrl_t*  tabctrl;
};

struct gui_tabpage_t
{
    gui_container_t base;   
};

struct gui_tabctrl_t
{
    gui_widget_t      base;
    gui_tabpage_t**   tabpages;
    uint32_t          tabpage_count;
    gui_tabbutton_t** btns;
    gui_tabpage_t*    selected_tab;
    int               selected_index;
};

gui_widget_t* gui_create_widget(int x, int y, int w, int h, char* text, gui_container_t* parent);

void gui_dispose_widget(gui_widget_t* widget);
void gui_update_widget(gui_widget_t* widget);
void gui_draw_widget(gui_widget_t* widget);
void gui_render_widget(gui_widget_t* widget);

void gui_event_refresh(gui_widget_t* widget);
void gui_event_ms_click(gui_widget_t* widget);
void gui_event_ms_down(gui_widget_t* widget);
void gui_event_ms_enter(gui_widget_t* widget);
void gui_event_ms_leave(gui_widget_t* widget);
void gui_event_ms_hover(gui_widget_t* widget);

void gui_set_text(gui_widget_t* widget, char* text);
void gui_set_tag(gui_widget_t* widget, char* tag);