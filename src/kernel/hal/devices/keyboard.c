#include <kernel/hal/devices/keyboard.h>
#include <kernel/core/kernel.h>

bool               _kbd_keymap[256];
uint8_t            _kbd_scancode;
kbd_handler_t*     _kbd_handler;

void kbd_init()
{
    memset(_kbd_keymap, 0, sizeof(_kbd_keymap));
    _kbd_scancode = 0;
    _kbd_handler = NULL;
}

void kbd_handle(uint8_t data)
{
    _kbd_scancode = data;
    kbd_update_keymap();
    kbd_update_handler();
}

void kbd_update_keymap()
{
    if (_kbd_scancode <= 0x58) { _kbd_keymap[_kbd_scancode] = true; }
    else if (_kbd_scancode >= 0x81 && _kbd_scancode < 0xD8) { _kbd_keymap[_kbd_scancode - 128] = false; }
}

void kbd_update_handler()
{
    if (_kbd_handler == NULL) { return; }

    if (_kbd_scancode == KEY_BACKSPACE)
    {
        if (strlen(_kbd_handler->stream.data) != NULL && _kbd_handler->stream.data != NULL)
        {
            strback(_kbd_handler->stream.data);
            if (_kbd_handler->on_backspace != NULL) { _kbd_handler->on_backspace(_kbd_handler->host, NULL); }
        }
    }
    else if (_kbd_scancode == KEY_LSHIFT) { _kbd_handler->left_shift = true; }
    else if (_kbd_scancode == KEY_RSHIFT) { _kbd_handler->right_shift = true; }
    else if (_kbd_scancode == 0xAA) { _kbd_handler->left_shift = false; }
    else if (_kbd_scancode == 0xB6) { _kbd_handler->right_shift = false; }
    else if (_kbd_scancode == KEY_CAPS_LOCK) { _kbd_handler->caps_lock = !_kbd_handler->caps_lock; }
    else if (_kbd_scancode == KEY_LCTRL) { }
    else if (_kbd_scancode == KEY_LALT) { }
    else if (_kbd_scancode == KEY_ESCAPE) { }
    else if (_kbd_scancode == KEY_ENTER) 
    { 
        if (_kbd_handler->newline && _kbd_handler->stream.data != NULL) { stradd(_kbd_handler->stream.data, '\n'); }
        if (_kbd_handler->on_enter != NULL) { _kbd_handler->on_enter(_kbd_handler->host, NULL); }
    }
    else if (_kbd_scancode < 58)
    {
        char ascii = 0;
        bool caps = (((_kbd_handler->left_shift || _kbd_handler->right_shift) && !_kbd_handler->caps_lock) || ((!_kbd_handler->left_shift && !_kbd_handler->right_shift) && _kbd_handler->caps_lock));

        if (caps) { ascii = KB_LAYOUT_US.upper[_kbd_scancode]; }
        else      { ascii = KB_LAYOUT_US.lower[_kbd_scancode]; }

        if (ascii >= 32 && ascii <= 126 && _kbd_handler->stream.data != NULL) { stradd(_kbd_handler->stream.data, ascii); }
        if (_kbd_handler->on_char != NULL) { _kbd_handler->on_char(_kbd_handler->host, ascii); }
    }
}

void kbd_set_handler(kbd_handler_t* handler) { _kbd_handler = handler; }

kbd_handler_t* kbd_get_handler() { return _kbd_handler; }

bool kbd_keydown(KEY key) { return _kbd_keymap[key]; }

bool kbd_keyup(KEY key) { return !_kbd_keymap[key]; }