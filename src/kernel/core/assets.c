#include <kernel/core/assets.h>
#include <kernel/core/kernel.h>

void assets_load()
{
    ASSETS.mscur_default = image_load("A:/mscur_default.bmp", IMGFORMAT_BMP);
    ASSETS.wallpaper     = image_load("A:/wallpaper.bmp", IMGFORMAT_BMP);
    ASSETS.icons32       = image_load("A:/icons32.bmp", IMGFORMAT_BMP);
    ASSETS.tbar          = image_load("A:/tbar.bmp", IMGFORMAT_BMP);

    ASSETS.icons16       = image_create(ASSETS.icons32.width, ASSETS.icons32.height);
    memcpy(ASSETS.icons16.data, ASSETS.icons32.data, ASSETS.icons32.width * ASSETS.icons32.height * 4);
    image_resize(&ASSETS.icons16, ASSETS.icons32.width / 2, ASSETS.icons32.height / 2);

    debug_ok("Loaded assets");
}