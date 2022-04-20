#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>

typedef struct
{
    void*    items;
    uint32_t item_size;
    uint32_t count;
} PACKED list_t;

list_t list_create(uint32_t item_size);
bool   list_clear(list_t* list);
bool   list_add(list_t* list, void* item);
bool   list_remove(list_t* list, void* item);