#pragma once
#include <lib/stdint.h>

typedef struct
{
    int x, y, width, height;
} PACKED rect_t;

static inline bool rect_contains(rect_t rect, int x, int y)
{
    return (x >= rect.x && y >= rect.y && x < rect.x + rect.width && y < rect.y + rect.height);
}