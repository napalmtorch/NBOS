#pragma once
#include <lib/stdint.h>
#include <lib/graphics/color.h>
#include <lib/graphics/font.h>
#include <lib/graphics/primitives.h>

typedef enum
{
    BORDERSTYLE_NONE,
    BORDERSTYLE_FLAT,
    BORDERSTYLE_3D,
} BORDERSTYLE;

typedef enum
{
    THEME_BG,
    THEME_BG_HOVER,
    THEME_BG_DOWN,
    THEME_FG,
    THEME_FG_HOVER,
    THEME_FG_DOWN,
    THEME_BORDER,
    THEME_BORDER_HOVER,
    THEME_BORDER_DOWN,
    THEME_BORDER_INNER,
    THEME_BORDER_OUTER,
    THEME_TOGGLED,
    THEME_DISABLED,
    THEME_TBAR_BG,
    THEME_TBAR_FG,
    THEME_TBAR_DISABLED,
} THEMEINDEX;

typedef struct
{
    char        name[32];
    BORDERSTYLE border_style;
    int         border_size;
    font_t*     font;
    uint32_t    colors[16];
} PACKED gui_theme_t;

static const gui_theme_t THEME_DEFAULT = (gui_theme_t) {
    .name         = "default",
    .border_style = BORDERSTYLE_3D,
    .border_size  = 1,
    .font         = &FONT_SANS8x14,
    .colors       =
    {
        COL32_LIGHTGRAY,
        COL32_SILVER,
        COL32_LIGHTGRAY,
        COL32_BLACK,
        COL32_BLACK,
        COL32_BLACK,
        COL32_WHITE,
        COL32_GRAY,
        COL32_GRAY,
        COL32_DIMGRAY,
        COL32_BLACK,
        COL32_DARKBLUE,
        COL32_GAINSBORO,
        COL32_DARKBLUE,
        COL32_WHITE,
        COL32_DIMGRAY,
    }
};

static const gui_theme_t THEME_DEFAULT_LABEL = (gui_theme_t) {
    .name         = "default",
    .border_style = BORDERSTYLE_3D,
    .border_size  = 1,
    .font         = &FONT_SANS8x14,
    .colors       =
    {
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_BLACK,
        COL32_BLACK,
        COL32_BLACK,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
    }
};

static const gui_theme_t THEME_DEFAULT_LISTBOX = (gui_theme_t) {
    .name         = "default",
    .border_style = BORDERSTYLE_3D,
    .border_size  = 1,
    .font         = &FONT_SANS8x14,
    .colors       =
    {
        COL32_WHITE,
        COL32_WHITE,
        COL32_WHITE,
        COL32_BLACK,
        COL32_WHITE,
        COL32_WHITE,
        COL32_GRAY,
        COL32_GRAY,
        COL32_GRAY,
        COL32_BLACK,
        COL32_DIMGRAY,
        COL32_DARKBLUE,
        COL32_GRAY,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
        COL32_TRANSPARENT,
    }
};