#pragma once
#include <lib/stdint.h>
#include <lib/graphics/image.h>

typedef struct
{
    image_t mscur_default;
    image_t mscur_caret;
    image_t mscur_grab;
    image_t mscur_hand;
    image_t mscur_wait;
    image_t icons32, icons16;
    image_t tbar;
    image_t wallpaper;
} assets_t;

void assets_load();