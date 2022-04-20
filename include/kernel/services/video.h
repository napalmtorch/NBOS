#pragma once
#include <lib/stdint.h>
#include <lib/gui/container.h>
#include <lib/graphics/image.h>
#include <kernel/exec/thread.h>

typedef struct taskbar_t taskbar_t;

struct taskbar_t
{
    gui_container_t base;
    gui_label_t*    label_time;
};


typedef struct
{
    uint16_t width;
    uint16_t height;
    uint8_t  depth;
} PACKED videomode_t;

typedef struct
{
    thread_t*    thread;
    videomode_t* supported_modes;
    uint32_t     supported_count;
    image_t      buffer;
    taskbar_t*   taskbar;
    int          fps, fps_limit;
    double       frametime;
    char         str_fps[64];
    char         str_ftime[64];
    char         str_wintop[64];
} PACKED videoctrl_t;

void video_init(videomode_t mode);
void video_setmode(int w, int h);
void video_fetchmodes();
videoctrl_t video_getinfo();