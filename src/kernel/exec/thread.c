#include <kernel/exec/thread.h>
#include <kernel/core/kernel.h>

extern void thread_switch();
void        thread_exit();
thread_t*   threadmgr_next();

// ---------------------------------------------------------------------------------------------------------------------------------------------
// --- THREAD MANAGER --------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------

void threadmgr_init()
{
    THREADMGR.count   = 0;
    THREADMGR.threads = NULL;
    THREADMGR.id      = 0;
    THREADMGR.ready   = false;
    THREADMGR.index   = 0;
    THREAD            = NULL;
    THREAD_NEXT       = NULL;

    thread_t* kthread = thread_create_kernel();
    thread_t* ithread = thread_create("idle", TSTACK_SMALL, idle_main, 0, NULL);
    threadmgr_load(kthread);
    threadmgr_load(ithread);
    THREAD = kthread;

    for (uint32_t i = 0; i < HEAP_LARGE.max; i++)
    {
        if (HEAP_LARGE.entries[i].size > 0 && HEAP_LARGE.entries[i].type != MEMTYPE_FREE && HEAP_LARGE.entries[i].thread == NULL) { HEAP_LARGE.entries[i].thread = kthread; }
    }

    for (uint32_t i = 0; i < HEAP_SMALL.max; i++)
    {
        if (HEAP_SMALL.entries[i].size > 0 && HEAP_SMALL.entries[i].type != MEMTYPE_FREE && HEAP_SMALL.entries[i].thread == NULL) { HEAP_SMALL.entries[i].thread = kthread; }
    }

    debug_ok("Initialized threading");
}

void yieldf() { unlock(); yield(); }

void yield()
{
    //debug_info("THREAD SWITCH");
    if (!THREADMGR.ready) { asm volatile("sti"); return; }
    if (THREADMGR.threads == NULL) { asm volatile("sti"); return; }

    asm volatile("cli");
    THREAD = THREADMGR.threads[THREADMGR.index];
    if (THREAD == NULL) { asm volatile("sti"); return; }
    if (THREAD->state == TSTATE_TERMINATED) { THREAD->locked = false; }
    if (THREAD->locked) { asm volatile("sti"); return; }

    THREAD_NEXT = threadmgr_next();
    thread_switch();
}

void tswitch(thread_t* thread)
{
    asm volatile("cli");
    unlock();
    if (thread == NULL || threadmgr_get_index(thread) == -1) { asm volatile("sti"); return; }
    THREADMGR.index = threadmgr_get_index(thread);
    THREAD_NEXT = thread;
    thread_switch();
}

thread_t* threadmgr_next()
{
    while (true)
    {
        THREADMGR.index++;
        if (THREADMGR.index >= THREADMGR.count) { THREADMGR.index = 0; return THREADMGR.threads[THREADMGR.index]; }
        if (THREADMGR.threads[THREADMGR.index] == NULL) { kpanic("Null thread detected in scheduler"); return NULL; }
        if (THREADMGR.threads[THREADMGR.index]->state == TSTATE_TERMINATED) { threadmgr_unload(THREADMGR.threads[THREADMGR.index]); continue; }
        if (THREADMGR.threads[THREADMGR.index]->state == TSTATE_RUNNING) { return THREADMGR.threads[THREADMGR.index]; }
    }
}

bool threadmgr_load(thread_t* thread)
{
    if (thread == NULL) { return false; }

    _cli();

    thread_t** list = tmalloc((THREADMGR.count + 1) * sizeof(thread_t*), MEMTYPE_PTRARRAY);
    if (THREADMGR.threads != NULL) { memcpy(list, THREADMGR.threads, THREADMGR.count * sizeof(thread_t*)); free(THREADMGR.threads); }
    
    THREADMGR.threads = list;
    THREADMGR.threads[THREADMGR.count] = thread;
    THREADMGR.count++;
    thread->state = TSTATE_RUNNING;

    debug_info("Loaded thread - ADDR: 0x%8x ID: %d NAME: '%s'", thread, thread->id, thread->name);
    _sti();
    return true;
}

bool threadmgr_unload(thread_t* thread)
{
    _cli();
    if (!threadmgr_locate(thread)) { _sti(); return false; }
    if (THREADMGR.count - 1 == 0)  { thread_dispose(THREADMGR.threads[0]); free(THREADMGR.threads); _sti(); return true; }

    thread_t** list = tmalloc((THREADMGR.count - 1) * sizeof(thread_t*), MEMTYPE_PTRARRAY);

    int pos = 0;
    for (uint32_t i = 0; i < THREADMGR.count; i++) { if (THREADMGR.threads[i] != thread) { list[pos] = THREADMGR.threads[i]; pos++; } }

    free(THREADMGR.threads);
    THREADMGR.threads = list;
    THREADMGR.count--;
    thread_dispose(thread);
    debug_info("Unloaded thread at 0x%8x", thread);
    _sti();
    return true;
}

void threadmgr_terminate(thread_t* thread)
{
    if (thread == NULL) { return; }
    thread->state = TSTATE_TERMINATED;
}

bool threadmgr_locate(thread_t* thread)
{
    if (thread == NULL) { return false; }
    for (uint32_t i = 0; i < THREADMGR.count; i++) { if (THREADMGR.threads[i] == thread) { return true; } }
    return false;
}

thread_t* threadmgr_get_byindex(int index)
{
    if (index < 0 || index >= THREADMGR.count) { return NULL; }
    return THREADMGR.threads[index];
}

int threadmgr_get_index(thread_t* thread)
{
    if (thread == NULL) { return -1; }
    for (int i = 0; i < THREADMGR.count; i++)
    {
        if (THREADMGR.threads[i] == thread) { return i; }
    }
    return -1;
}

uint32_t threadmgr_generate_id()
{
    uint32_t id = THREADMGR.id;
    THREADMGR.id++;
    return id;
}

void lock()
{
    if (THREAD == NULL) {  return; }
    THREAD->locked = true;
}

void unlock()
{
    if (THREAD == NULL) { return; }
    THREAD->locked = false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// --- THREAD ----------------------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------------------------------

thread_t* thread_create_kernel()
{
    thread_t* t = (thread_t*)tmalloc(sizeof(thread_t), MEMTYPE_THREAD);
    heapfe(t)->thread = t;
    
    thread_setname(t, "kernel");
    t->id = threadmgr_generate_id();
    t->entry = kernel_main;
    t->stack = (uint32_t)&_stack_bottom;
    t->stack_size = (uint32_t)&_stack_top - (uint32_t)&_stack_bottom;
    t->state = TSTATE_RUNNING;
    
    debug_ok("Created thread - ADDR: 0x%8x, ID: %d, STACK(0x%8x-0x%8x), ENTRY: 0x%8x, NAME: '%s'", t, t->id, t->stack, t->stack + t->stack_size, t->entry, t->name);
    return t;
}

thread_t* thread_create(char* name, uint32_t stack_size, thread_entry_t entry, int argc, char** argv)
{
    thread_t* t = tmalloc(sizeof(thread_t), MEMTYPE_THREAD);

    thread_setname(t, name);
    t->stack_size = clamp(stack_size, TSTACK_SMALL, TSTACK_LARGE);
    t->stack      = tmalloc(t->stack_size, MEMTYPE_ARRAY);
    t->id         = threadmgr_generate_id();
    t->locked     = false;
    t->state      = TSTATE_HALTED;
    t->entry      = entry;
    t->args_count = argc + 1;
    t->args       = tmalloc(t->args_count * sizeof(char*), MEMTYPE_PTRARRAY);    
    t->args[0]    = t;
    for (uint32_t i = 0; i < argc; i++) { t->args[i + 1] = argv[i]; }

    uint32_t* stk = (uint32_t*)(t->stack + (t->stack_size - sizeof(thread_regs_t) - 20));
    *--stk = (uint32_t)t;
    *--stk = (uint32_t)t->args;
    *--stk = (uint32_t)t->args_count;
    *--stk = (uint32_t)thread_exit;
    *--stk = (uint32_t)entry;
    t->registers.esp    = stk;
    t->registers.eflags = 0x202;

    heapfe(t)->thread = t;
    heapfe(t->stack)->thread = t;
    heapfe(t->args)->thread = t;
    debug_ok("Created thread - ADDR: 0x%8x, ID: %d, STACK(0x%8x-0x%8x), ENTRY: 0x%8x, NAME: '%s'", t, t->id, t->stack, t->stack + t->stack_size, t->entry, t->name);
    return t;
}

void thread_dispose(thread_t* thread)
{
    if (thread == NULL) { return; }
    free(thread->stack);
    free(thread->args);
    free(thread);
}

void thread_monitor(thread_t* thread)
{
    if (thread == NULL) { return; }
    thread->ticks++;
    thread->time = pit_get_seconds();
    if (thread->time != thread->timelast)
    {
        thread->timelast = thread->time;
        thread->tps      = thread->ticks;
        thread->ticks    = 0;
    }
}

void thread_setname(thread_t* thread, char* name)
{
    if (thread == NULL || name == NULL) { return; }

    uint32_t len = strlen(name);
    for (uint32_t i = 0; i < len; i++)
    {
        if (i < sizeof(thread->name)) { thread->name[i] = name[i]; }
        else { kpanic("Thread name too long - '%s'", name); return; }
    }
}

void thread_exit()
{
    _cli();

    register int eax asm("eax");
    uint32_t code = eax;
    THREAD->state = TSTATE_TERMINATED;
    debug_info("Thread %d('%s') exited with code %d", THREAD->id, THREAD->name, code);
    _sti();
    while (true) { yield(); }
}