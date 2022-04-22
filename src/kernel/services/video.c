#include <kernel/services/video.h>
#include <kernel/core/kernel.h>

const char  _vidname[] = "shellhost";
videoctrl_t _vidctrl;
int         _vidtime, _vidtimelast, _vidframes;
int         _drawtime, _drawtimelast, _drawtimer;
int         _last_wincount;
gui_window_t* _last_winactive;

int video_main(int argc, char** argv);
void video_draw();
void video_update_strings();

void taskbar_draw(taskbar_t* taskbar);
void taskbar_update_time(taskbar_t* taskbar);

void video_init(videomode_t mode)
{
    _vidctrl.buffer.data = NULL;
    vbe_init();
    _vidctrl.thread      = thread_create(_vidname, TSTACK_DEFAULT, video_main, 0, NULL);
    _vidctrl.fps_limit   = 300;
    video_fetchmodes();
    video_setmode(mode.width, mode.height);

    _last_wincount = 1000000;
    _last_winactive = NULL;

    threadmgr_load(_vidctrl.thread);
    debug_ok("Initialized video driver");
}

int video_main(int argc, char** argv)
{
    lock(); 
    debug_ok("Entered video driver main"); 
    image_resize(&ASSETS.wallpaper, KARGS.video_mode.width, KARGS.video_mode.height);
    winmgr_init();

    _vidctrl.taskbar = gui_create_container(0, KARGS.video_mode.height - 22, KARGS.video_mode.width, 22, "taskbar", NULL);
    _vidctrl.taskbar->label_time = gui_create_label(_vidctrl.taskbar->base.base.bounds.width - 124, 0, "12:00 AM", _vidctrl.taskbar);
    gui_container_add_widget(_vidctrl.taskbar, _vidctrl.taskbar->label_time);
    taskbar_update_time(_vidctrl.taskbar);

    gui_window_t* win2 = term_create(32, 32);
    gui_window_t* win = taskmgr_create(256, 64);
    winmgr_load(win2);
    winmgr_load(win);

    unlock();

    while (true)
    {        
        lock();
        thread_monitor(argv[0]);

        // 1-second timer
        _vidtime = pit_get_seconds();
        if (_vidtime != _vidtimelast)
        {
            _vidtimelast = _vidtime;
            _vidctrl.fps = _vidframes;
            _vidctrl.frametime = ceil((double)(1000.0 / (double)_vidctrl.fps));
            _vidframes = 0;
            video_update_strings();

            taskbar_update_time(_vidctrl.taskbar);
        }

        if (_last_wincount != winmgr_get_count())
        {
            _last_wincount = winmgr_get_count();
            taskbar_winbtns_update(_vidctrl.taskbar);
        }

        if (_last_winactive != winmgr_get_active())
        {
            _last_winactive = winmgr_get_active();
            taskbar_winbtns_update_active(_vidctrl.taskbar);
        }

        // draw timer increment
        _drawtime = pit_get_millis();
        if (_drawtime != _drawtimelast) { _drawtimelast = _drawtime; _drawtimer++; }

        // on draw timer tick
        int limit = 0;
        if (_vidctrl.fps_limit > 0) { limit = 1000 / _vidctrl.fps_limit; }
        if (_drawtimer >= limit || _vidctrl.fps_limit == 0)
        {
            if (_vidctrl.fps_limit > 0) { _drawtimer = 0; }
            _vidframes++;
            video_draw();
            unlock();
            if (_vidctrl.fps_limit > 0) { yield(); }
        }
        
        // limit wasted cpu time
        if ((_vidctrl.fps_limit > 0 && _drawtimer < limit) || (_drawtimer >= 16 && _vidctrl.fps_limit == 0)) { yieldf(); }
    }
    
    image_dispose(&_vidctrl.buffer);
    vbe_clear(0);
    return 0;
}

void video_draw()
{
    // background
    if (ASSETS.wallpaper.data == NULL) { image_clear(&_vidctrl.buffer, 0xFF007F7F); }
    else { memcpy(_vidctrl.buffer.data, ASSETS.wallpaper.data, ASSETS.wallpaper.width * ASSETS.wallpaper.height * 4); }

    // window manager
    winmgr_update();
    winmgr_render();

    // taskbar
    taskbar_draw(_vidctrl.taskbar);

    // debug text
    image_string(&_vidctrl.buffer, 0, 0,  _vidctrl.str_fps,   COL32_WHITE, COL32_BLACK, FONT_SANS8x14);
    image_string(&_vidctrl.buffer, 0, 14, _vidctrl.str_ftime, COL32_WHITE, COL32_BLACK, FONT_SANS8x14);
    image_string(&_vidctrl.buffer, 0, 28, _vidctrl.str_wintop, COL32_WHITE, COL32_BLACK, FONT_SANS8x14);

    // mouse
    if (ASSETS.mscur_default.data != NULL) { image_copy_trans(&_vidctrl.buffer, mouse_getx(), mouse_gety(), ASSETS.mscur_default.width, ASSETS.mscur_default.height, COL32_MAGENTA, ASSETS.mscur_default.data); }

    // copy buffer to vbe framebuffer
    memcpy(vbe_getbuff(), _vidctrl.buffer.data, KARGS.video_mode.width * KARGS.video_mode.height * 4);
}

void video_update_strings()
{
    char temp[32];
    memset(_vidctrl.str_fps, 0, sizeof(_vidctrl.str_fps));
    memset(_vidctrl.str_ftime, 0, sizeof(_vidctrl.str_ftime));
    memset(_vidctrl.str_wintop, 0, sizeof(_vidctrl.str_wintop));

    memset(temp, 0, 32);
    strcat(_vidctrl.str_fps, "FPS: ");
    strcat(_vidctrl.str_fps, ltoa(_vidctrl.fps, temp, 10));

    memset(temp, 0, 32);
    strcat(_vidctrl.str_ftime, "FRAMETIME: ");
    strcat(_vidctrl.str_ftime, ltoa((int)_vidctrl.frametime, temp, 10));
    strcat(_vidctrl.str_ftime, "ms");

    memset(temp, 0, 32);
    strcat(_vidctrl.str_wintop, "TOP_WIN: ");
    strcat(_vidctrl.str_wintop, itoa(_win_top, temp, 10));
}

void video_setmode(int w, int h)
{
    for (int i = 0; i < _vidctrl.supported_count; i++)
    {
        if (_vidctrl.supported_modes[i].width == w && _vidctrl.supported_modes[i].height == h && _vidctrl.supported_modes[i].depth == 32)
        {
            debug_info("Setting video mode to %dx%dx%dbpp", w, h, 32);
            KARGS.video_mode = (videomode_t){ .width = w, .height = h, .depth = 32 };
            vbe_setmode(w, h); 
            image_dispose(&_vidctrl.buffer);
            _vidctrl.buffer = image_create(w, h);
            return; 
        }
    }

    debug_error("Unable to set video mode %dx%dx%dbpp", w, h, 32);
}

void video_fetchmodes()
{
    if (_vidctrl.supported_modes != NULL) { free(_vidctrl.supported_modes); }
    int count = 0;
    videomode_t* modes       = vbe_fetchmodes(&count);
    _vidctrl.supported_modes = modes;
    _vidctrl.supported_count = count;
}

videoctrl_t video_getinfo() { return _vidctrl; }

// -----------------------------------------------------------------------

void taskbar_draw(taskbar_t* taskbar)
{
    taskbar->base.base.update(taskbar);
    taskbar->base.base.render(taskbar);
}

void taskbar_update_time(taskbar_t* taskbar)
{
    char* time = timestr(timenow(), false, true);
    int tw = font_getsw(taskbar->label_time->base.theme.font, time);
    taskbar->label_time->base.bounds.x = taskbar->base.base.bounds.width - (tw + 12);
    taskbar->label_time->base.settext(taskbar->label_time, time);
    free(time);
}

void taskbar_winbtns_update(taskbar_t* taskbar)
{
    if (taskbar == NULL) { return; }

    if (taskbar->winbtns != NULL)
    {
        for (int i = 0; i < taskbar->winbtns_count; i++) 
        { 
            gui_container_remove_widget_at(taskbar, i);
            taskbar->winbtns[i]->base.dispose(taskbar->winbtns[i]); 
        }
        free(taskbar->winbtns);
    }

    taskbar->winbtns_count = winmgr_get_count();
    taskbar->winbtns = tmalloc(taskbar->winbtns_count * sizeof(gui_button_t*), MEMTYPE_PTRARRAY);

    int xx = 48, yy = 2;
    for (int i = 0; i < taskbar->winbtns_count; i++)
    {
        taskbar->winbtns[i] = gui_create_button(xx, yy, 120, taskbar->base.base.bounds.height - 5, winmgr_get_list()[i]->base.base.text, taskbar);
        taskbar->winbtns[i]->can_toggle = true;
        taskbar->winbtns[i]->base.on_ms_click = taskbar_winbtn_down;
        free(taskbar->base.base.tag);
        taskbar->winbtns[i]->base.tag = winmgr_get_list()[i];
        if (winmgr_get_list()[i] == winmgr_get_active()) { taskbar->winbtns[i]->base.flags.toggled = true; }
        gui_container_add_widget(taskbar, taskbar->winbtns[i]);
        xx += 122;
    }
    taskbar->base.base.draw(taskbar);
}

void taskbar_winbtns_update_active(taskbar_t* taskbar)
{
    if (winmgr_get_active() == NULL)
    { for (int i = 0; i < taskbar->winbtns_count; i++) { taskbar->winbtns[i]->base.flags.toggled = false; } taskbar->base.base.draw(taskbar); return; }

    for (int i = 0; i < taskbar->winbtns_count; i++)
    {
        if (winmgr_get_active() == taskbar->winbtns[i]->base.tag) { taskbar->winbtns[i]->base.flags.toggled = true; }
        else { taskbar->winbtns[i]->base.flags.toggled = false; }
    }
    taskbar->base.base.draw(taskbar);
}

void taskbar_winbtn_down(gui_button_t* btn)
{
    gui_event_ms_click(btn);
    for (int i = 0; i < winmgr_get_count(); i++)
    {
        if (btn->base.tag == winmgr_get_list()[i])
        {
            winmgr_set_active(winmgr_get_list()[i]);
            return;
        }
    }
}