#include <kernel/memory/heap.h>
#include <kernel/core/kernel.h>

heap_entry_t* heap_create_allocated(heap_t* heap, uint32_t size);
heap_entry_t* heap_nearest(heap_t* heap, heap_entry_t* entry);
heap_entry_t* heap_create(heap_t* heap, uint32_t addr, uint32_t size, uint8_t type);
bool          heap_delete(heap_t* heap, heap_entry_t* entry);
void          heap_merge(heap_t* heap);
int           heap_free_index(heap_t* heap);

heap_t heap_init(uint32_t size, uint32_t count, uint32_t align, bool msg)
{
    mem_entry_t* entries = mem_request(count * sizeof(heap_entry_t), MEMTYPE_HEAPTABLE);
    mem_entry_t* data    = mem_request(size, MEMTYPE_HEAP);

    heap_t heap = (heap_t){ .entries = entries->address, .data = data->address, .size = size, .max = count, .alignment = align, .messages = msg };
    heap_create(&heap, heap.data, heap.size, MEMTYPE_FREE);
    debug_ok("Initialized heap - ENTRIES: 0x%8x DATA: 0x%8x ALIGN: 0x%8x, SIZE: %d MB", heap.entries, heap.data, heap.alignment, heap.size / 1024 / 1024);
    return heap;
}

int heap_cleart(int argc, char** argv)
{
    lock();
    heap_t* heap = argv[1];
    if (heap == NULL) { return 1; }
    int count = 0;
    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].ptr == NULL || heap->entries[i].size == 0 || heap->entries[i].type == MEMTYPE_RESERVED || heap->entries[i].thread == NULL) { continue; }
        if (heap->entries[i].type != MEMTYPE_FREE) 
        { 
            if (!threadmgr_exists(heap->entries[i].thread)) { heap_free(heap, heap->entries[i].ptr); count++; }
        }
    }
    heap_merge(heap);
    //debug_info("Freed %d dead pointers", count);
    unlock();
    return 0;
}

void heap_clean(heap_t* heap)
{
    heap->time = pit_get_seconds();
    if (heap->time != heap->timelast) { heap->timelast = heap->time; heap->timer++; }

    if (heap->timer >= 3)
    {          
        lock();
        heap->timer = 0;
        char** newargs = tmalloc(1 * sizeof(char*), MEMTYPE_PTRARRAY);
        newargs[0] = heap;

        thread_t* t = thread_create("memclean", TSTACK_DEFAULT, heap_cleart, 1, newargs);
        threadmgr_load(t);
        unlock();
    }
}

void* heap_allocate(heap_t* heap, uint32_t size, uint8_t type)
{
    if (size == 0) { return NULL; }
    heap_entry_t* entry = heap_create_allocated(heap, size);
    if (entry == NULL) { kpanic("System out of memory - REQ SIZE: %d bytes", size); return NULL; }

    entry->thread = THREAD;
    entry->type   = (type < MEMTYPE_ALLOC ? MEMTYPE_ALLOC : type);
    memset(entry->ptr, 0, entry->size);

    if (heap->messages)
    {
        debug_header("MALLOC", COL4_GREEN);
        debug_printf("ALIGN: 0x%8x, ADDR: 0x%8x THREAD: 0x%8x, TYPE: %s  SIZE: %d bytes\n", heap->alignment, entry->ptr, entry->thread, mem_typestr(entry->type), entry->size);
    }
    return entry->ptr;
}

bool heap_free(heap_t* heap, void* ptr)
{
    if (heap == NULL || ptr == NULL) { return; }

    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].ptr == ptr)
        {
            MEMTYPE type = heap->entries[i].type;
            heap->entries[i].type = MEMTYPE_FREE;

            if (heap->messages)
            {
                debug_header(" FREE ", COL4_YELLOW);
                debug_printf("ALIGN: 0x%8x, ADDR: 0x%8x TYPE: %s  SIZE: %d bytes\n", heap->alignment, heap->entries[i].ptr, mem_typestr(type), heap->entries[i].size);
            }
            heap_merge(heap);
            return true;
        }
    }
    return false;
}

heap_entry_t* heap_create_allocated(heap_t* heap, uint32_t size)
{
    if (size == 0) { return NULL; }
    uint32_t aligned = mem_align(size, heap->alignment);

    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].size >= aligned && heap->entries[i].type == MEMTYPE_FREE)
        {
            heap_entry_t* entry = heap_create(heap, heap->entries[i].ptr, aligned, MEMTYPE_ALLOC);
            heap->entries[i].size -= aligned;
            heap->entries[i].ptr  += aligned;
            return entry;
        }
    }
    return NULL;
}

heap_entry_t* heap_create(heap_t* heap, uint32_t addr, uint32_t size, uint8_t type)
{
    if (!heap_validate_addr(heap, addr) || size == 0) { return NULL; }
    int i = heap_free_index(heap);

    heap->entries[i] = (heap_entry_t){ .ptr = addr, .size = size, .type = type, .thread = NULL };
    heap->count++;
    return &heap->entries[i];
}

bool heap_delete(heap_t* heap, heap_entry_t* entry)
{
    if (heap == NULL || entry == NULL) { return false; }

    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (&heap->entries[i] == entry)
        {
            memset(&heap->entries[i], 0, sizeof(heap_entry_t));
            return true;
        }
    }
    return false;
}

heap_entry_t* heap_nearest(heap_t* heap, heap_entry_t* entry)
{
    if (heap == NULL || entry == NULL) { return NULL; }

    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].type != MEMTYPE_FREE) { continue; }
        if (heap->entries[i].ptr + heap->entries[i].size == entry->ptr) { return &heap->entries[i]; }
        if (entry->ptr - entry->size == heap->entries[i].ptr) { return &heap->entries[i]; }
    }
    return NULL;
}

void heap_merge(heap_t* heap)
{
    if (heap == NULL) { return; }
    uint32_t count = 0;

    heap_entry_t* nearest = NULL;
    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].type != MEMTYPE_FREE) { continue; }
        
        nearest = heap_nearest(heap, &heap->entries[i]);
        if (nearest != NULL)
        {
            if (heap->entries[i].ptr > nearest->ptr) { heap->entries[i].ptr = nearest->ptr; }
            heap->entries[i].size += nearest->size;
            heap_delete(heap, nearest);
            count++;
        }
    }

    if (heap->entries[0].type == MEMTYPE_FREE)
    {
        nearest = heap_nearest(heap, &heap->entries[0]);
        if (nearest != NULL)
        {
            if (heap->entries[0].ptr > nearest->ptr) { heap->entries[0].ptr = nearest->ptr; }
            heap->entries[0].size += nearest->size;
            heap_delete(heap, nearest);
            count++;
        }
    }
}

uint32_t heap_calc_used(heap_t* heap, thread_t* thread)
{
    uint32_t bytes = 0;
    for (int i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].type != MEMTYPE_FREE && heap->entries[i].size > 0) 
        { 
            if (thread != NULL && heap->entries[i].thread != thread) { continue; }
            bytes += heap->entries[i].size; 
        }
    }
    return bytes;
}

heap_entry_t* heap_fetch(heap_t* heap, void* ptr)
{
    if (ptr == NULL || heap == NULL) { return NULL; }

    for (int i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].ptr == ptr && heap->entries[i].size > 0 && heap->entries[i].type != MEMTYPE_FREE) { return &heap->entries[i]; }
    }
    return NULL;
}

bool heap_validate_addr(heap_t* heap, uint32_t addr)
{
    if (heap == NULL) { return false; }
    if (addr >= heap->data && addr < heap->data + heap->size) { return true; }
    return false;
}

int heap_free_index(heap_t* heap)
{
    for (int i = 0; i < heap->max; i++) { if (heap->entries[i].ptr == NULL && heap->entries[i].size == 0) { return i; } }
    return -1;
}

void heap_print(heap_t* heap)
{
    for (uint32_t i = 0; i < heap->max; i++)
    {
        if (heap->entries[i].ptr != NULL)
        {
            debug_info("Heap entry - ADDR: 0x%8x, TYPE: %s  SIZE: %d bytes", heap->entries[i].ptr, mem_typestr(heap->entries[i].type), heap->entries[i].size);
        }
    }
}