#include <lib/queue.h>
#include <kernel/core/kernel.h>

queue_t queue_create(uint32_t item_size)
{
    queue_t q = (queue_t){ .items = NULL, .item_size = item_size, .position = 0 };
    return q;
}

bool queue_clear(queue_t* queue)
{
    if (queue == NULL) { return false; }
    if (queue->items != NULL) { free(queue->items); queue->items = NULL; }
    queue->position = 0;
}

bool queue_enqueue(queue_t* queue, void* item)
{
    if (queue == NULL) { return false; }
    void* items = tmalloc((queue->position + 1) * queue->item_size, MEMTYPE_ARRAY);

    if (queue->items != NULL)
    {
        memcpy(items, queue->items, queue->position * queue->item_size);
        free(queue->items);
    }

    queue->items = items;
    memcpy((void*)(queue->items + (queue->position * queue->item_size)), item, queue->item_size);
    queue->position++;
    return true;
}

void* queue_dequeue(queue_t* queue)
{

}

bool queue_isdone(queue_t* queue)
{
    if (queue == NULL) { return true; }
    if (queue->items == NULL) { return true; }
    if (queue->position < 0) { return true; }
    return false;
}

strqueue_t strqueue_create()
{
    strqueue_t q = (strqueue_t){ .items = NULL, .item_size = sizeof(char*), .position = 0 };
    return q;
}

bool strqueue_enqueue(strqueue_t* queue, char* item)
{
    if (queue == NULL) { return false; }
    char** items = tmalloc((queue->position + 1) * queue->item_size, MEMTYPE_ARRAY);

    if (queue->items != NULL)
    {
        memcpy(items, queue->items, queue->position * queue->item_size);
        free(queue->items);
    }

    queue->items = items;
    queue->items[queue->position] = item;
    queue->position++;
    return true;
}

char* strqueue_dequeue(strqueue_t* queue)
{
    if (queue_isdone(queue)) { return NULL; }
    if (queue->position - 1 < 0) { return NULL; }

    char* item = queue->items[0];
    if (queue->position - 1 == 0) { queue_clear(queue); return item; }
    char** items = tmalloc((queue->position - 1) * queue->item_size, MEMTYPE_PTRARRAY);
    for (int i = 1; i < queue->position; i++) { items[i - 1] = queue->items[i]; }
    free(queue->items);
    queue->items = items;
    queue->position--;
    return item;
}