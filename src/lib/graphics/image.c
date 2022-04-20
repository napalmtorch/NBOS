#include <lib/graphics/image.h>
#include <kernel/core/kernel.h>

image_t image_create(int w, int h)
{
    image_t img;
    img.width  = clamp(w, 1, UINT16_MAX);
    img.height = clamp(h, 1, UINT16_MAX);
    img.data   = tmalloc(img.width * img.height * 4, MEMTYPE_FRMBUFF);
    debug_ok("Created image - SIZE: %dx%d, DATA: 0x%8x", img.width, img.height, img.data);
    return img;
}

image_t image_create_existing(int w, int h, uint32_t* data)
{

}

image_t image_load(char* fname, IMGFORMAT format)
{
    image_t image = (image_t){ .width = 0, .height = 0, .data = NULL };

    if (format == IMGFORMAT_BMP)
    {
        file_t file = vfs_file_open(fname);
        if (file.data == NULL) { return image; }

        bitmap_filehdr_t* fhdr    = file.data;
        bitmap_infohdr_t* ihdr    = file.data + sizeof(bitmap_filehdr_t);
        uint32_t          offset  = fhdr->off_bits;
        uint8_t*          bmpdata = file.data + offset;
        uint8_t           bpp     = ihdr->bit_count;
        uint32_t          size    = ihdr->width * ihdr->height * 4;

        image.width  = ihdr->width;
        image.height = ihdr->height;
        image.format = IMGFORMAT_BMP;
        image.data   = tmalloc(size, MEMTYPE_FRMBUFF);

        for (int yy = image.height - 1; yy >= 0; yy--)
        {
            for (int xx = 0; xx < image.width; xx++)
            {
                if (bpp == 24)
                {
                    uint32_t offset = (3 * (xx + (yy * image.width)));
                    uint32_t color = RGBToCOL32(bmpdata[offset + 2], bmpdata[offset + 1], bmpdata[offset]);
                    uint32_t output_offset = xx + ((image.height - yy - 1) * image.width);
                    image.data[output_offset] = color;
                }
                else if (bpp == 32)
                {
                    uint32_t offset = (4 * (xx + (yy * image.width)));
                    uint32_t color = RGBToCOL32(bmpdata[offset + 2], bmpdata[offset + 1], bmpdata[offset]);
                    uint32_t output_offset = xx + ((image.height - yy - 1) * image.width);
                    image.data[output_offset] = color;
                }
            }
        }  
    }
    return image;
}

void image_dispose(image_t* image)
{
    if (image == NULL) { return; }
    if (image->data != NULL) { free(image->data); }
}

void image_resize(image_t* img, int w, int h)
{
    if (img == NULL) { return; }
    if (img->data == NULL) { return; }
    int old_w = img->width, old_h = img->height;

    uint32_t* temp = (uint32_t*)tmalloc(w * h * sizeof(uint32_t), MEMTYPE_FRMBUFF);
    double x_ratio = (double)img->width / (double)w;
    double y_ratio = (double)img->height / (double)h;
    int px, py;
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            px = floor(j * x_ratio);
            py = floor(i * y_ratio);
            temp[(i * w) + j] = ((uint32_t*)img->data)[(int)((py * img->width) + px)];
        }
    }
    if (img->data != NULL) { free(img->data); }
    img->data = (uint32_t*)temp;
    img->width = w;
    img->height = h;
    debug_info("Resized image from %dx%d to %dx%d", old_w, old_h, w, h);
}

void image_clear(image_t* img, uint32_t color)
{
    memset(img->data, color, img->width * img->height * 4);
}

void image_blit(image_t* img, int x, int y, uint32_t color)
{
    if (img == NULL) { return; }
    if ((uint32_t)x >= img->width || (uint32_t)y >= img->height) { return; }
    img->data[y * img->width + x] = color;
}

void image_rectf(image_t* img, int x, int y, int w, int h, uint32_t color)
{
    if (color == COL32_TRANSPARENT) { return; }
    register int i;
    for (i = 0; i < w * h; i++)
    {
        if ((uint32_t)(x + (i % w)) >= img->width || (uint32_t)(y + (i / w)) >= img->height) { continue; }
        img->data[(y + (i / w)) * img->width + (x + (i % w))] = color;
    }
}

void image_rect(image_t* img, int x, int y, int w, int h, int t, uint32_t color)
{
    if (color == COL32_TRANSPARENT) { return; }
    image_rectf(img, x, y, w, t, color);
    image_rectf(img, x, y + h - t, w, t, color);
    image_rectf(img, x, y + t, t, h - (t * 2), color);
    image_rectf(img, x + w - t, y + t, t, h - (t * 2), color);
}

void image_rect3d(image_t* img, int x, int y, int w, int h, bool inward, uint32_t ca, uint32_t cb, uint32_t cc)
{
    if (inward)
    {
        image_rectf(img, x, y, w - 1, 1, cc);
        image_rectf(img, x, y, 1, h - 1, cc);
        image_rectf(img, x + 1, y + 1, w - 3, 1, cb);
        image_rectf(img, x + 1, y + 1, 1, h - 3, cb);
        image_rectf(img, x, y + h - 1, w, 1, ca);
        image_rectf(img, x + w - 1, y, 1, h, ca);
    }
    else
    {
        image_rectf(img, x, y, w, 1, ca);
        image_rectf(img, x, y, 1, h, ca);
        image_rectf(img, x + 1, y + h - 2, w - 2, 1, cb);
        image_rectf(img, x + w - 2, y + 1, 1, h - 2, cb);
        image_rectf(img, x, y + h - 1, w, 1, cc);
        image_rectf(img, x + w - 1, y, 1, h, cc);
    }
}

void image_char(image_t* img, int x, int y, char c, uint32_t fg, uint32_t bg, font_t font)
{
    if (bg != COL32_TRANSPARENT) { image_rectf(img, x, y, font_getw(&font), font_geth(&font), bg); }
    uint32_t p = font.height * c;

    register int i;
    while (i < font.width * font.height)
    {
        register int xx = x + (font.width - (i % font.width));
        register int yy = y + (i / font.width);
        if (bit_addr_from_byte(font.data[p + (i / font.width)], (i % font.width) + 1)) { image_blit(img, xx, yy, fg); }
        i++;
    }
}

void image_string(image_t* img, int x, int y, char* str, uint32_t fg, uint32_t bg, font_t font)
{
    register int i;
    int xx = x, yy = y;
    while (str[i] != 0)
    {
        if (str[i] == '\n') { xx = x; yy += font_geth(&font); }
        else { image_char(img, xx, yy, str[i], fg, bg, font); xx += font_getw(&font); }
        i++;
    }
}

void image_copy_trans(image_t* img, int x, int y, int w, int h, uint32_t trans, uint32_t* data)
{
    for (uint32_t i = 0; i < w * h; i++)
    {
        if (data[i] == trans) { continue; }
        if ((uint32_t)(x + (i % w)) >= img->width || (uint32_t)(y + (i / w)) >= img->height) { continue; }
        img->data[(y + (i / w)) * img->width + (x + (i % w))] = data[i];
    }
}

void image_copy(image_t* img, int x, int y, int w, int h, uint32_t* data)
{
    uint32_t iw = w;
    while (x + w > (int)img->width) { w--; }
    while (y + h > (int)img->height) { h--; }

    int xx, yy = 0;
    for (yy = 0; yy < h; yy++)
    {
        uint8_t* src = (uint8_t*)(data + (yy * iw));
        xx = x;
        if (xx < 0) { xx = 0; }
        while (xx + w > (int)img->width) { xx--; }
        uint32_t real_offset = (xx + ((y + yy) * img->width)) * 4;
        uint8_t* dest = (uint8_t*)((uint32_t)img->data + real_offset);
        if (y + yy >= 0 && (uint32_t)dest >= (uint32_t)img->data && (uint32_t)dest < (uint32_t)((uint32_t)img->data + (img->width * img->height * 4)))
        {
            if (x >= 0) { memcpy(dest, src, w * 4); }
            else { memcpy(dest, src - (x * 4), (w + x) * 4); }
        }
    }
}

void image_copy_partial(image_t* img, int x, int y, int img_w, int sx, int sy, int sw, int sh, uint32_t trans, uint32_t* data)
{
    if (img == NULL) { return; }
    for (int i = 0; i < sw * sh; i++)
    {
        if (data[(sy + (i / sw)) * img_w + (sx + (i % sw))] != (uint32_t)trans) { image_blit(img, x + (i % sw), y + (i / sw), data[(sy + (i / sw)) * img_w + (sx + (i % sw))]); }
    }
}

void image_copy_partialf(image_t* img, int x, int y, int img_w, int sx, int sy, int sw, int sh, uint32_t trans, uint32_t color, uint32_t* data)
{
    if (img == NULL) { return; }
    for (int i = 0; i < sw * sh; i++)
    {
        if (data[(sy + (i / sw)) * img_w + (sx + (i % sw))] != (uint32_t)trans) { image_blit(img, x + (i % sw), y + (i / sw), color); }
    }
}