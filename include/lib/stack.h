#pragma once
#include <lib/stdint.h>

typedef struct
{
    void*    items;
    uint32_t item_size;
    uint32_t position;
} PACKED stack_t;

typedef struct
{
    char*    items;
    uint32_t item_size;
    uint32_t position;
} PACKED charstack_t;

typedef struct
{
    char**   items;
    uint32_t item_size;
    uint32_t position;
} PACKED strstack_t;

stack_t stack_create(uint32_t item_size);
bool    stack_clear(stack_t* stack);
bool    stack_push(stack_t* stack, void* item);
bool    stack_remove(stack_t* stack, void* item);
bool    stack_remove_at(stack_t* stack, int index);
void*   stack_pop(stack_t* stack);
bool    stack_isdone(stack_t* stack);


charstack_t charstack_create();
bool        charstack_clear(charstack_t* stack);
bool        charstack_push(charstack_t* stack, char c);
bool        charstack_remove_at(charstack_t* stack, int index);
char        charstack_pop(charstack_t* stack);

strstack_t strstack_create();
bool       strstack_clear(strstack_t* stack);
bool       strstack_push(strstack_t* stack, char* str);
char*      strstack_pop(strstack_t* stack);
