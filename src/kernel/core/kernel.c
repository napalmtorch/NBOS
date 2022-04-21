#include <kernel/core/kernel.h>

void kernel_entry(void* mboot)
{
    MULTIBOOT = mboot;

    kernel_boot();
    kernel_main();
}

void kernel_boot()
{
    serial_setport(SERIALPORT_COM1);
    debug_setmode(DEBUG_SERIAL);
    debug_ok("Initialized serial connection on %s", serial_getportstr(serial_getport()));

    gdt_init();
    idt_init();
    _sti();

    mem_init();
    HEAP_LARGE = heap_init(256 * 1024 * 1024, 4096,  0x1000, false);
    HEAP_SMALL = heap_init(256 * 1024 * 1024, 16384, 0x100, false);

    kernel_parse_args();

    threadmgr_init();

    vfs_init();
    assets_load();

    video_init(KARGS.video_mode);

    cmdhandler_init();

    rtc_init();

    ps2_init();

    //gs_tokenizer_t toks = gs_tokenize_file("A:/test.gs");

    pit_init(5000);
    THREADMGR.ready = true;
}

void kernel_main()
{
    lock(); debug_ok("Entered kernel main"); unlock();

    cmdhandler_execute("cls", NULL);

    while (true)
    {
        lock();
        thread_monitor(threadmgr_get_byindex(0));
        heap_clean(&HEAP_SMALL);
        heap_clean(&HEAP_LARGE);
        yieldf();
    }
}

int idle_main(int argc, char** argv)
{
    lock(); debug_ok("Entered idle main"); unlock();
    while (true) { lock(); thread_monitor(argv[0]); yieldf(); }
}

void kernel_parse_args()
{
    int    argc = 0;
    char** argv = strsplit(MULTIBOOT->command_line, ' ', &argc);

    int i = 0;
    while (true)
    {
        if (i >= argc) { break; }
        if (!strcmp(argv[i], "-debug"))
        {
            if (!strcmp(argv[i + 1], "none")   || !strcmp(argv[i + 1], "0"))      { debug_setmode(DEBUG_DISABLED); }
            else if (!strcmp(argv[i + 1], "serial") || !strcmp(argv[i + 1], "1")) { debug_setmode(DEBUG_SERIAL); }
            else if (!strcmp(argv[i + 1], "term")   || !strcmp(argv[i + 1], "2")) { debug_setmode(DEBUG_TERMINAL); }
            else if (!strcmp(argv[i + 1], "all")    || !strcmp(argv[i + 1], "3")) { debug_setmode(DEBUG_ALL); }
            else { debug_error("Unknown debug mode '%s'", argv[i + 1]); }
            i += 2;
        }
        else if (!strcmp(argv[i], "-width"))  { KARGS.video_mode.width  = atol(argv[i + 1]); i += 2; }
        else if (!strcmp(argv[i], "-height")) { KARGS.video_mode.height = atol(argv[i + 1]); i += 2; }
        else if (!strcmp(argv[i], "-depth"))  { KARGS.video_mode.depth  = atol(argv[i + 1]); i += 2; }
        else { debug_error("Unknown kernel argument - '%s'", argv[i]); return; }
    }
    freearray(argv, argc);
}

uint32_t kernel_get_size() { return mem_align((uint32_t)((uint32_t)&_kernel_end - (uint32_t)&_kernel_start), 0x1000); }