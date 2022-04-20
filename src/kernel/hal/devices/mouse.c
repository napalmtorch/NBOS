#include <kernel/hal/devices/mouse.h>
#include <kernel/core/kernel.h>

int32_t        _mouse_x, _mouse_y, _mouse_cycle;
mouse_packet_t _mouse_packet;
bool           _mouse_handled;

void mouse_move(int8_t x, int8_t y)
{
    _mouse_x += x;
    _mouse_y += y;
    if (_mouse_x < 0) { _mouse_x = 0; }
    if (_mouse_y < 0) { _mouse_y = 0; }
    if (_mouse_x > KARGS.video_mode.width - 1) { _mouse_x = KARGS.video_mode.width - 1; }
    if (_mouse_y > KARGS.video_mode.height - 1) { _mouse_y = KARGS.video_mode.height - 1; }
}

void mouse_handle(uint8_t data)
{
    _mouse_handled = false;

    switch (_mouse_cycle)
    {
        case 0:
        {
            _mouse_packet.flags.value = data;
            if (!(data & 0x08)) { return; }
            _mouse_cycle++;
            break;
        }
        case 1: { _mouse_packet.delta_x = data; _mouse_cycle++; break; }
        case 2:
        {
            if (mouse_overflow()) { _mouse_cycle = 0; return; }
            _mouse_packet.delta_y = -data;
            _mouse_handled        = true;
            _mouse_cycle          = 0;
            break;
        }
    }

    if (_mouse_handled)
    {
        mouse_move(_mouse_packet.delta_x, _mouse_packet.delta_y);
    }
}

bool mouse_overflow()
{
    return (_mouse_packet.flags.bits.overflow_x || _mouse_packet.flags.bits.overflow_y);
}

int mouse_getx() { return _mouse_x; }

int mouse_gety() { return _mouse_y; }

bool mouse_btn_left() { return _mouse_packet.flags.bits.btn_left; }

bool mouse_btn_right() { return _mouse_packet.flags.bits.btn_right; }

bool mouse_btn_middle() { return _mouse_packet.flags.bits.btn_middle; }