#pragma once
#include <lib/stdint.h>

typedef struct
{
    void*    items;
    uint32_t item_size;
    uint32_t position;
} PACKED queue_t;

typedef struct
{
    char*    items;
    uint32_t item_size;
    uint32_t position;
} PACKED charqueue_t;

typedef struct
{
    char**   items;
    uint32_t item_size;
    uint32_t position;
} PACKED strqueue_t;

queue_t queue_create(uint32_t item_size);
bool    queue_clear(queue_t* queue);
bool    queue_enqueue(queue_t* queue, void* item);
void*   queue_dequeue(queue_t* queue);
bool    queue_isdone(queue_t* queue);

strqueue_t strqueue_create();
bool       strqueue_enqueue(strqueue_t* queue, char* item);
char*      strqueue_dequeue(strqueue_t* queue);

