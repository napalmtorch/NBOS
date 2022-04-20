#include <lib/list.h>
#include <kernel/core/kernel.h>

list_t list_create(uint32_t item_size)
{
    list_t list = (list_t){ .count = 0, .items = NULL, .item_size = item_size };
    return list;
}

bool list_clear(list_t* list)
{
    if (list->items != NULL) { free(list->items); list->items = NULL; }
    list->count = 0;
}

bool list_add(list_t* list, void* item)
{
    if (list == NULL) { return false; }
    void* items = tmalloc((list->count + 1) * list->item_size, MEMTYPE_ARRAY);

    if (list->items != NULL)
    {
        memcpy(items, list->items, list->count * list->item_size);
        free(list->items);
    }

    list->items = items;
    memcpy((void*)(list->items + (list->count * list->item_size)), item, list->item_size);
    list->count++;
    return true;
}

bool list_remove(list_t* list, void* item)
{
    for (uint32_t i = 0; i < list->count; i++)
    {
        if (!memcmp(list->items + (i * list->item_size), item, list->item_size))
        {
            if (list->count - 1 == 0) { list_clear(list); return true; }
            void* items = tmalloc((list->count - 1) * list->item_size, MEMTYPE_ARRAY);
            int pos = 0;
            for (uint32_t j = 0; j < list->count; j++)
            {
                if (j != i) { memcpy((void*)(items + (pos * list->item_size)), (void*)(list->items + (j * list->item_size)), list->item_size); pos++; }
            }
            free(list->items);
            list->items = items;
            list->count--;
            return true;
        }
    }
    return false;
}