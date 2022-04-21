#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <lib/stddef.h>
#include <kernel/exec/thread.h>

typedef struct
{
    void*     ptr;
    thread_t* thread;
    uint32_t  size;
    uint8_t   type;
} PACKED heap_entry_t;

typedef struct
{
    heap_entry_t* entries;
    uint32_t      data;
    uint32_t      size;
    uint32_t      count;
    uint32_t      max;
    uint32_t      alignment;
    uint32_t      time, timelast, timer;
    bool          messages;
} PACKED heap_t;

heap_t heap_init(uint32_t size, uint32_t count, uint32_t align, bool msg);
void heap_print(heap_t* heap);
void heap_clean(heap_t* heap);

void* heap_allocate(heap_t* heap, uint32_t size, uint8_t type);
bool  heap_free(heap_t* heap, void* ptr);

uint32_t heap_calc_used(heap_t* heap, thread_t* thread);

heap_entry_t* heap_fetch(heap_t* heap, void* ptr);

bool heap_validate_addr(heap_t* heap, uint32_t addr);