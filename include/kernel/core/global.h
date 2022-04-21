#pragma once
#include <lib/stdarg.h>
#include <lib/stddef.h>
#include <lib/stdarg.h>
#include <lib/stdlib.h>
#include <lib/stdint.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <lib/ctype.h>
#include <lib/list.h>
#include <lib/stack.h>
#include <lib/queue.h>
#include <lib/expr.h>
#include <lib/math.h>
#include <lib/graphics/color.h>
#include <lib/graphics/font.h>
#include <lib/graphics/image.h>
#include <lib/gui/widget.h>
#include <lib/gui/container.h>
#include <lib/gui/tabctrl.h>
#include <lib/gui/tabpage.h>
#include <lib/gui/button.h>
#include <lib/gui/label.h>
#include <lib/gui/listbox.h>
#include <lib/gui/scrollbar_y.h>
#include <lib/gui/window.h>
#include <lib/gui/winmgr.h>
#include <lib/gui/theme.h>
#include <lib/graphics/primitives.h>
#include <kernel/core/multiboot.h>
#include <kernel/core/debug.h>
#include <kernel/core/assets.h>
#include <kernel/hal/common/ports.h>
#include <kernel/hal/common/serial.h>
#include <kernel/hal/common/realmode.h>
#include <kernel/hal/common/pit.h>
#include <kernel/hal/common/rtc.h>
#include <kernel/hal/interrupts/gdt.h>
#include <kernel/hal/interrupts/idt.h>
#include <kernel/hal/devices/vga.h>
#include <kernel/hal/devices/vbe.h>
#include <kernel/hal/devices/ps2.h>
#include <kernel/hal/devices/mouse.h>
#include <kernel/hal/devices/keyboard.h>
#include <kernel/memory/memmgr.h>
#include <kernel/memory/heap.h>
#include <kernel/exec/thread.h>
#include <kernel/exec/command.h>
#include <kernel/exec/gsharp/tokenizer.h>
#include <kernel/services/video.h>
#include <kernel/services/terminal.h>
#include <kernel/services/taskmgr.h>
#include <kernel/fs/ramfs.h>
#include <kernel/fs/vfs.h>

typedef struct
{
    videomode_t video_mode;
    DEBUG       debug_mode;
} PACKED kernel_args_t;

extern multiboot_t*  MULTIBOOT;
extern kernel_args_t KARGS;

extern heap_t HEAP_LARGE;
extern heap_t HEAP_SMALL;

extern threadmgr_t THREADMGR;

extern thread_t* THREAD;
extern thread_t* THREAD_NEXT;

extern ramfs_t RAMFS;

extern assets_t ASSETS;

static inline heap_entry_t* heapfe(void* ptr)
{
    heap_entry_t* entry = heap_fetch(&HEAP_LARGE, ptr);
    if (entry != NULL) { return entry; }
    return heap_fetch(&HEAP_SMALL, ptr);
}
