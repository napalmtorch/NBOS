#include <kernel/core/global.h>
#include <kernel/core/kernel.h>

multiboot_t* MULTIBOOT;
kernel_args_t KARGS;

heap_t HEAP_LARGE;
heap_t HEAP_SMALL;

threadmgr_t THREADMGR;
thread_t*   THREAD;
thread_t*   THREAD_NEXT;

ramfs_t RAMFS;

assets_t ASSETS;