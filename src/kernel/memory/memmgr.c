#include <kernel/memory/memmgr.h>
#include <kernel/core/kernel.h>

#define MEM_COUNT 4096

mem_entry_t _mem_entries[MEM_COUNT];
uint32_t    _mem_count;
uint32_t    _mem_start;
uint32_t    _mem_size;

int mem_free_index();
bool mem_valid_index(int i);

void mem_init()
{
    memset(&_mem_entries, 0, sizeof(mem_entry_t) * MEM_COUNT);
    _mem_count = 0;

    mmap_entry_t* mmap = MULTIBOOT->mmap_addr;
    for (int i = 0; i < MULTIBOOT->mmap_len / sizeof(mmap_entry_t); i++)
    {
        if (mmap[i].type == MEMTYPE_FREE && mmap[i].address_low < (uint32_t)&_kernel_start) { continue; }
        if (mmap[i].address_low == (uint32_t)&_kernel_start) { _mem_size = mmap[i].length_low; continue; }
        mem_map(mmap[i].address_low, mmap[i].length_low, mmap[i].type);
    }

    uint32_t mods_end  = 0, mods_size = 0;
    multiboot_module_t* mods = MULTIBOOT->modules_addr;
    for (int i = 0; i < MULTIBOOT->modules_count; i++)
    {
        mem_entry_t* entry = mem_map(mods[i].address_start, mods[i].address_end - mods[i].address_start, MEMTYPE_MODULE);
        if (mods[i].address_end > mods_end) { mods_end = mem_align(mods[i].address_end, 0x1000); }
        mods_size += mem_align(entry->size, 0x1000);
    }

    if (mods_end > 0) { _mem_start = mods_end; _mem_size -= (mods_size + kernel_get_size()); }
    else { _mem_start = (uint32_t)&_kernel_end + 0x1000; _mem_size -= kernel_get_size(); }

    mem_map((uint32_t)&_kernel_start, kernel_get_size(), MEMTYPE_KERNEL);
    mem_map(_mem_start, _mem_size, MEMTYPE_FREE);

    debug_ok("Initialized memory manager");
}

mem_entry_t* mem_map(uint32_t addr, uint32_t size, uint8_t type)
{
    if (size == 0) { return NULL; }
    
    int index = mem_free_index();
    if (!mem_valid_index(index)) { return NULL; }

    _mem_entries[index] = (mem_entry_t){ .address = addr, .size = size, .type = type };
    _mem_count++;
    debug_header(" MMAP ", COL4_MAGENTA);
    debug_printf("ADDR: 0x%8x TYPE: %s  SIZE: %d bytes\n", addr, mem_typestr(type), size);
    return &_mem_entries[index];
}

mem_entry_t* mem_request(uint32_t size, MEMTYPE type)
{
    if (type == MEMTYPE_UNUSABLE || type == MEMTYPE_FREE || type == MEMTYPE_INVALID) { kpanic("Attempt to request memory with type %s", mem_typestr(type)); return NULL; }

    for (int i = 0; i < MEM_COUNT; i++)
    {
        if (_mem_entries[i].size == 0) { continue; }
        if (_mem_entries[i].size >= size && _mem_entries[i].type == MEMTYPE_FREE)
        {
            int index = mem_free_index();
            if (!mem_valid_index(index)) { return NULL; }

            _mem_entries[index] = (mem_entry_t){ .address = _mem_entries[i].address, .size = size, .type = type };
            _mem_count++;

            debug_header(" MREQ ", COL4_BLUE);
            debug_printf("ADDR: 0x%8x TYPE: %s  SIZE: %d bytes\n", _mem_entries[index].address, mem_typestr(type), size);
            
            _mem_entries[i].address += size;
            _mem_entries[i].size    -= size;

            memset((void*)_mem_entries[index].address, 0, _mem_entries[index].size);
            return &_mem_entries[index];
        }
    }

    kpanic("Failed to request %d bytes of memory", size);
    return NULL;
}

bool mem_unmap(mem_entry_t entry)
{
    if (entry.size == 0) { return false; }

    for (int i = 0; i < MEM_COUNT; i++)
    {
        if (!memcmp(&entry, &_mem_entries[i], sizeof(mem_entry_t)))
        {
            debug_printf("MUNMAP", COL4_YELLOW);
            debug_printf("ADDR: 0x%8x TYPE: %s  SIZE: %d bytes\n", _mem_entries[i].address, mem_typestr(_mem_entries[i].type), _mem_entries[i].size);
            memset(&_mem_entries[i], 0, sizeof(mem_entry_t));
            _mem_count--;
            return true;
        }
    }
    return false;
}

uint32_t mem_align(uint32_t value, uint32_t align)
{
    uint32_t out = value;
    out &= (0xFFFFFFFF - (align - 1));
    if (out < value) { out += align; }
    return out;
}

int mem_free_index()
{
    for (int i = 0; i < MEM_COUNT; i++) { if (_mem_entries[i].address == 0 && _mem_entries[i].size == 0) { return i; } }
    return -1;
}

bool mem_valid_index(int i)
{
    return (i >= 0 && i < MEM_COUNT);
}