#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <lib/stddef.h>

#define TSTACK_SMALL   0x2000
#define TSTACK_DEFAULT 0x10000
#define TSTACK_LARGE   0x80000

typedef enum
{
    TSTATE_HALTED,
    TSTATE_RUNNING,
    TSTATE_TERMINATED,
} TSTATE;

typedef int (*thread_entry_t)(int argc, char** argv);

typedef struct
{
    uint32_t esp, ebp, esi, edi;
    uint32_t eax, ebx, ecx, edx;
    uint32_t eflags;
} PACKED thread_regs_t;

typedef struct
{
    thread_regs_t  registers;
    thread_entry_t entry;
    TSTATE         state;
    bool           locked;
    void*          stack;
    uint32_t       stack_size;
    char**         args;
    uint32_t       args_count;
    uint32_t       tps, ticks, time, timelast;
    uint32_t       id;
    char           name[256];
} PACKED thread_t;

typedef struct
{
    thread_t** threads;
    uint32_t   count;
    uint32_t   id;
    uint32_t   index;
    bool       ready;
} PACKED threadmgr_t;

void     threadmgr_init();
void     yieldf();
void     yield();
void     tswitch(thread_t* t);
bool     threadmgr_load(thread_t* thread);
bool     threadmgr_unload(thread_t* thread);
void     threadmgr_terminate(thread_t* thread);
bool     threadmgr_locate(thread_t* thread);
thread_t* threadmgr_get_byindex(int index);
uint32_t threadmgr_generate_id();
int      threadmgr_get_index(thread_t* thread);

thread_t* thread_create_kernel();
thread_t* thread_create(char* name, uint32_t stack_size, thread_entry_t entry, int argc, char** argv);
void      thread_dispose(thread_t* thread);
void      thread_monitor(thread_t* thread);

void thread_setname(thread_t* thread, char* name);

void lock();
void unlock();