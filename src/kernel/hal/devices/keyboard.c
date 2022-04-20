#include <kernel/hal/devices/keyboard.h>
#include <kernel/core/kernel.h>

bool               _kbd_keymap[256];
uint8_t            _kbd_scancode;
keyboard_handler_t* _kbd_handler;

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
    
}

void kbd_set_handler(keyboard_handler_t* handler) { _kbd_handler = handler; }

bool kbd_keydown(KEY key) { return _kbd_keymap[key]; }

bool kbd_keyup(KEY key) { return !_kbd_keymap[key]; }