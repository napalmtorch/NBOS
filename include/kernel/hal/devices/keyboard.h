#pragma once
#include <lib/stdint.h>
#include <kernel/hal/interrupts/idt.h>

typedef enum
{
KEY_INVALID,
KEY_ESCAPE,
KEY_D1,
KEY_D2,
KEY_D3,
KEY_D4,
KEY_D5,
KEY_D6,
KEY_D7,
KEY_D8,
KEY_D9,
KEY_D0,
KEY_MINUS,
KEY_EQUAL,
KEY_BACKSPACE,
KEY_TAB,
KEY_Q,
KEY_W,
KEY_E,
KEY_R,
KEY_T,
KEY_Y,
KEY_U,
KEY_I,
KEY_O,
KEY_P,
KEY_LBRACKET,
KEY_RBRACKET,
KEY_ENTER,
KEY_LCTRL,
KEY_A,
KEY_S,
KEY_D,
KEY_F,
KEY_G,
KEY_H,
KEY_J,
KEY_K,
KEY_L,
KEY_COLON,
KEY_QUOTE,
KEY_BACKTICK,
KEY_LSHIFT,
KEY_BACKSLASH,
KEY_Z,
KEY_X,
KEY_C,
KEY_V,
KEY_B,
KEY_N,
KEY_M,
KEY_COMMA,
KEY_PERIOD,
KEY_SLASH,
KEY_RSHIFT,
KEY_KP_MULTIPLY,
KEY_LALT,
KEY_SPACE,
KEY_CAPS_LOCK,
KEY_F1,
KEY_F2,
KEY_F3,
KEY_F4,
KEY_F5,
KEY_F6,
KEY_F7,
KEY_F8,
KEY_F9,
KEY_F10,
KEY_NUM_LOCK,
KEY_SCROLL_LOCK,
KEY_KP_HOME,
KEY_KP_UP,
KEY_KP_PGUP,
KEY_KEY_KP_MINUS,
KEY_KP_LEFT,
KEY_KP_D5,
KEY_KP_RIGHT,
KEY_KP_PLUS,
KEY_KP_END,
KEY_KP_DOWN,
KEY_KP_PGDN,
KEY_KP_PERIOD,
KEY_F11 = 0x57,
KEY_F12,
} KEY;

typedef void (*keyboard_event_t)(void* sender, void* arg);

typedef struct
{
    bool left_shift  : 1;
    bool right_shift : 1;
    bool control     : 1;
    bool alt         : 1;
    bool caps_lock   : 1;
    bool escape      : 1;
    bool newline     : 1;
    keyboard_event_t on_char;
    keyboard_event_t on_enter;
    keyboard_event_t on_backspace;
} PACKED keyboard_handler_t;

void kbd_init();
void kbd_handle(uint8_t data);
void kbd_update_keymap();
void kbd_update_handler();
void kbd_set_handler(keyboard_handler_t* handler);

bool kbd_keydown(KEY key);
bool kbd_keyup(KEY key);