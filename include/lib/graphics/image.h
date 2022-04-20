#pragma once
#include <lib/stdint.h>
#include <lib/graphics/font.h>

typedef enum
{
    IMGFORMAT_BMP,
    IMGFORMAT_PNG,
    IMGFORMAT_JPEG,
} IMGFORMAT;

typedef struct
{
    uint16_t type;
    uint32_t size;
    uint16_t reserved0;
    uint16_t reserved1;
    uint32_t off_bits;
} PACKED bitmap_filehdr_t;

typedef struct
{
    uint32_t size;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;
    uint32_t x_ppm;
    uint32_t y_ppm;
    uint32_t clr_used;
    uint32_t clr_important;
} bitmap_infohdr_t;

typedef struct
{
    int       width, height;
    uint32_t* data;
    IMGFORMAT format;
} PACKED image_t;

image_t image_create(int w, int h);
image_t image_create_existing(int w, int h, uint32_t* data);
image_t image_load(char* fname, IMGFORMAT format);
void    image_dispose(image_t* image);
void    image_resize(image_t* img, int w, int h);

void image_clear(image_t* img, uint32_t color);
void image_blit(image_t* img, int x, int y, uint32_t color);
void image_rectf(image_t* img, int x, int y, int w, int h, uint32_t color);
void image_rect(image_t* img, int x, int y, int w, int h, int t, uint32_t color);
void image_rect3d(image_t* img, int x, int y, int w, int h, bool inward, uint32_t ca, uint32_t cb, uint32_t cc);
void image_char(image_t* img, int x, int y, char c, uint32_t fg, uint32_t bg, font_t font);
void image_string(image_t* img, int x, int y, char* str, uint32_t fg, uint32_t bg, font_t font);
void image_copy_trans(image_t* img, int x, int y, int w, int h, uint32_t trans, uint32_t* data);
void image_copy(image_t* img, int x, int y, int w, int h, uint32_t* data);
void image_copy_partial(image_t* img, int x, int y, int img_w, int sx, int sy, int sw, int sh, uint32_t trans, uint32_t* data);
void image_copy_partialf(image_t* img, int x, int y, int img_w, int sx, int sy, int sw, int sh, uint32_t trans, uint32_t color, uint32_t* data);