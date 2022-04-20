#include <kernel/hal/interrupts/gdt.h>
#include <kernel/core/kernel.h>

#define KERNEL_CS_INDEX 1
#define KERNEL_DS_INDEX 2

const gdt_access_t KERNEL_CS_ACCESS = (gdt_access_t)
{
    .accessed        = 0,
    .read_write      = 1,
    .dc_flag         = GDT_NONCONF,
    .executable      = true,
    .type            = 1,
    .privilege_level = 0,
    .present         = 1,
};

const gdt_access_t KERNEL_DS_ACCESS = (gdt_access_t)
{
    .accessed        = 0,
    .read_write      = 1,
    .dc_flag         = GDTDIR_UPWARDS,
    .executable      = false,
    .type            = 1,
    .privilege_level = 0,
    .present         = 1,
};

const gdt_flags_t KERNEL_CS_FLAGS = (gdt_flags_t)
{
    .long_mode   = false,
    .size        = GDTSIZE_32BIT,
    .granularity = GDTGRAN_ALIGN_4K,
};

const gdt_flags_t KERNEL_DS_FLAGS = (gdt_flags_t)
{
    .long_mode   = false,
    .size        = GDTSIZE_32BIT,
    .granularity = GDTGRAN_ALIGN_4K,
};

uint32_t      _gdtr;
gdtr_t        gdtr ALIGNED(0x100);
gdt_entry_t   gdt[GDT_COUNT] ALIGNED(0x100);

extern void _gdt_flush();

void gdt_init()
{
    memset(&gdt, 0, sizeof(gdt_entry_t) * GDT_COUNT);
    
    _gdtr      = (uint32_t)&gdtr; 
    gdtr.base  = (uint32_t)gdt;
    gdtr.limit = (GDT_COUNT * sizeof(gdt_entry_t)) - 1;

    gdt_init_entries();

    _gdt_flush();
    debug_ok("Finished generating GDT");
}

void gdt_init_entries()
{
    gdt_set_desc(0, 0, 0, (gdt_access_t){ 0, 0, 0, 0, 0, 0, 0 }, (gdt_flags_t){ 0, 0, 0, 0 });
    gdt_set_desc(KERNEL_CS_INDEX, 0, 0xFFFFFFFF, KERNEL_CS_ACCESS, KERNEL_CS_FLAGS);
    gdt_set_desc(KERNEL_DS_INDEX, 0, 0xFFFFFFFF, KERNEL_DS_ACCESS, KERNEL_DS_FLAGS);
}

void gdt_set_desc(uint8_t n, uint32_t base, uint32_t limit, gdt_access_t access, gdt_flags_t flags)
{
    gdt[n].base_low = base & 0xFFFF;
	gdt[n].base_middle = (base >> 16) & 0xFF;
	gdt[n].base_high = (base >> 24) & 0xFF;
	
	gdt[n].limit_low = limit & 0xFFFF;
	gdt[n].limit_high = (limit >> 16) & 0x0F;
	
	gdt[n].accessed = access.accessed;
	gdt[n].read_write = access.read_write;
	gdt[n].dc_flag = access.dc_flag;
	gdt[n].executable = access.executable;
	gdt[n].type = access.type;
	gdt[n].privilege_level = access.privilege_level;
	gdt[n].present = access.present;
	
	gdt[n].reserved = 0;
	gdt[n].long_mode = flags.long_mode;
	gdt[n].size = flags.size;
	gdt[n].granularity = flags.granularity;

    debug_info("GDT[%d] - BASE: 0x%8x LIMIT: 0x%8x ACCESS: 0x%2x FLAGS: 0x%2x", n, base, limit, access, flags);
}