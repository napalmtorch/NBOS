#include <lib/stack.h>
#include <kernel/core/kernel.h>

stack_t stack_create(uint32_t item_size)
{
    stack_t stack = (stack_t){ .items = NULL, .position = 0, .item_size = item_size };
    return stack;
}

bool stack_clear(stack_t* stack)
{
    if (stack == NULL) { return false; }
    if (stack->items != NULL) { free(stack->items); stack->items = NULL; }
    stack->position = 0;
}

bool stack_push(stack_t* stack, void* item)
{
    if (stack == NULL) { return false; }
    void* items = tmalloc((stack->position + 1) * stack->item_size, MEMTYPE_ARRAY);

    if (stack->items != NULL)
    {
        memcpy(items, stack->items, stack->position * stack->item_size);
        free(stack->items);
    }

    stack->items = items;
    memcpy((void*)(stack->items + (stack->position * stack->item_size)), item, stack->item_size);
    stack->position++;
    return true;
}

bool stack_remove(stack_t* stack, void* item)
{
    for (uint32_t i = 0; i < stack->position; i++)
    {
        if (!memcmp(stack->items + (i * stack->item_size), item, stack->item_size))
        {
            if (stack->position - 1 == 0) { stack_clear(stack); return true; }
            void* items = tmalloc((stack->position - 1) * stack->item_size, MEMTYPE_ARRAY);
            int pos = 0;
            for (uint32_t j = 0; j < stack->position; j++)
            {
                if (j != i) { memcpy((void*)(items + (pos * stack->item_size)), (void*)(stack->items + (j * stack->item_size)), stack->item_size); pos++; }
            }
            free(stack->items);
            stack->items = items;
            stack->position--;
            return true;
        }
    }
    return false;
}

bool stack_remove_at(stack_t* stack, int index)
{
    if (stack == NULL) { return false; }
    if (index < 0 || index >= stack->position) { return false; }
    if (stack->position == 0) { return stack_clear(stack); }

    void* data = tmalloc((stack->position - 1) * stack->item_size, MEMTYPE_ARRAY);

    int pos = 0;
    for (int i = 0; i < stack->position; i++)
    {
        if (i == index) { continue; }
        memcpy((void*)(data + (pos * stack->item_size)), (void*)(stack->items + (i * stack->item_size)), stack->item_size);
        pos++;
    }

    free(stack->items);
    stack->items = data;
    stack->position--;
    return true;
}

void* stack_pop(stack_t* stack)
{
    if (stack == NULL) { return NULL; }
    if (stack->items == NULL) { return NULL; }
    if (stack->position - 1 < 0) { return NULL; }
    void* val = tmalloc(stack->item_size, MEMTYPE_OBJECT);
    memcpy(val, (void*)(stack->items + ((stack->position - 1) * stack->item_size)), stack->item_size);
    stack_remove(stack, val);
    return val;
}

bool stack_isdone(stack_t* stack)
{
    if (stack == NULL) { return true; }
    if (stack->items == NULL) { return true; }
    if (stack->position < 0) { return true; }
    return false;
}


charstack_t charstack_create()
{
    charstack_t stack = (charstack_t){ .items = NULL, .item_size = sizeof(char), .position = 0 };
    return stack;
}

bool charstack_clear(charstack_t* stack)
{
    return stack_clear(stack);
}

bool charstack_push(charstack_t* stack, char c)
{
    if (stack == NULL) { return false; }
    char* items = tmalloc((stack->position + 1) * stack->item_size, MEMTYPE_ARRAY);

    if (stack->items != NULL)
    {
        memcpy(items, stack->items, stack->position * stack->item_size);
        free(stack->items);
    }

    stack->items = items;
    stack->items[stack->position] = c;
    stack->position++;
    return true;
}

bool charstack_remove_at(charstack_t* stack, int index)
{
    return stack_remove_at(stack, index);
}

char charstack_pop(charstack_t* stack)
{
    if (stack == NULL || stack->position < 0) { return 0; }

    if (stack->position - 1 == 0)
    {
        char c = stack->items[stack->position - 1];
        charstack_clear(stack);
        return c;
    }

    char* items = tmalloc((stack->position - 1) * stack->item_size, MEMTYPE_ARRAY);
    char c = stack->items[stack->position - 1];
    for (int i = 0; i < stack->position; i++) { items[i] = stack->items[i]; }
    free(stack->items);
    stack->items = items;
    stack->position--;
    return c;

}

strstack_t strstack_create()
{
    strstack_t stack = (strstack_t){ .items = NULL, .item_size = sizeof(char*), .position = 0 };
    return stack;
}

bool strstack_clear(strstack_t* stack)
{
    return stack_clear(stack);
}

bool strstack_push(strstack_t* stack, char* str)
{
    void* val = &str;
    return stack_push(stack, val);
}

char* strstack_pop(strstack_t* stack)
{
    char** v = stack_pop(stack);   
    char* str = *v;
    return str;
}