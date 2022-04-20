#pragma once
#include <lib/stdint.h>
#include <kernel/hal/interrupts/idt.h>

typedef struct
{
    bool btn_left   : 1;
    bool btn_right  : 1;
    bool btn_middle : 1;
    bool reserved   : 1;
    bool sign_x     : 1;
    bool sign_y     : 1;
    bool overflow_x : 1;
    bool overflow_y : 1;
} PACKED mouse_packet_flagbits_t;;

typedef union
{
    uint8_t                 value;
    mouse_packet_flagbits_t bits;
} mouse_packet_flags_t;

typedef struct
{
    int8_t               delta_x;
    int8_t               delta_y;
    mouse_packet_flags_t flags;
} mouse_packet_t;

void mouse_move(int8_t x, int8_t y);
void mouse_handle(uint8_t data);
bool mouse_overflow();

int mouse_getx();
int mouse_gety();
bool mouse_btn_left();
bool mouse_btn_right();
bool mouse_btn_middle();