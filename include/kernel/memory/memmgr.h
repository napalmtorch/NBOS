#pragma once
#include <lib/stdint.h>

typedef enum
{
    MEMTYPE_INVALID,
    MEMTYPE_FREE,
    MEMTYPE_RESERVED,
    MEMTYPE_RECLAIMABLE,
    MEMTYPE_ACPI,
    MEMTYPE_UNUSABLE,
    MEMTYPE_KERNEL,
    MEMTYPE_MODULE,
    MEMTYPE_HEAP,
    MEMTYPE_HEAPTABLE,

    MEMTYPE_ALLOC,
    MEMTYPE_FRMBUFF,
    MEMTYPE_STRING,
    MEMTYPE_PROCESS,
    MEMTYPE_THREAD,
    MEMTYPE_SERVICE,
    MEMTYPE_ARRAY,
    MEMTYPE_PTRARRAY,
    MEMTYPE_OBJECT,
    MEMTYPE_GUI,
    MEMTYPE_WINDOW,
} MEMTYPE;

static const char* mem_typestr(MEMTYPE type)
{
    switch (type)
    {
        case MEMTYPE_FREE:          { return "FREE     "; }
        case MEMTYPE_RESERVED:      { return "RESERVED "; }
        case MEMTYPE_RECLAIMABLE:   { return "RECLAIM  "; }
        case MEMTYPE_ACPI:          { return "ACPI     "; }
        case MEMTYPE_UNUSABLE:      { return "UNUSABLE "; }
        case MEMTYPE_KERNEL:        { return "KERNEL   "; }
        case MEMTYPE_MODULE:        { return "MODULE   "; }
        case MEMTYPE_HEAP:          { return "HEAP     "; }
        case MEMTYPE_HEAPTABLE:     { return "HEAPTABLE"; }
        case MEMTYPE_ALLOC:         { return "ALLOC    "; }
        case MEMTYPE_FRMBUFF:       { return "FRMBUFF  "; }
        case MEMTYPE_STRING:        { return "STRING   "; }
        case MEMTYPE_PROCESS:       { return "PROCESS  "; }
        case MEMTYPE_THREAD:        { return "THREAD   "; }
        case MEMTYPE_SERVICE:       { return "SERVICE  "; }
        case MEMTYPE_ARRAY:         { return "ARRAY    "; }
        case MEMTYPE_PTRARRAY:      { return "PTRARRAY "; }
        case MEMTYPE_OBJECT:        { return "OBJECT   "; }
        case MEMTYPE_GUI:           { return "GUI      "; }
        case MEMTYPE_WINDOW:        { return "WINDOW   "; }
        default:                    { return "INVALID  "; }
    }
}

typedef struct
{
    uint32_t address;
    uint32_t size;
    uint8_t  type;
} mem_entry_t;

void         mem_init();
mem_entry_t* mem_map(uint32_t addr, uint32_t size, uint8_t type);
mem_entry_t* mem_request(uint32_t size, MEMTYPE type);
bool         mem_unmap(mem_entry_t entry);

uint32_t mem_align(uint32_t value, uint32_t align);