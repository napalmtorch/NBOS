#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>

typedef enum 
{
	GDTDIR_UPWARDS,
	GDTDIR_DOWNWARDS,
} GDTDIR;

typedef enum
{
    GDTGRAN_ALIGN_1B,
    GDTGRAN_ALIGN_4K,
} GDTGRAN;

typedef enum
{
    GDTSIZE_16BIT,
    GDTSIZE_32BIT,
} GDTSIZE;

typedef struct
{
	uint8_t accessed        : 1;
	uint8_t read_write      : 1;
	bool    dc_flag         : 1;
	uint8_t executable      : 1;
	uint8_t type            : 1;
	uint8_t privilege_level : 2;
	uint8_t present         : 1;
} PACKED gdt_access_t;

typedef struct
{
	uint8_t reserved    : 1;
	bool    long_mode   : 1;
	GDTSIZE size        : 1;
	GDTGRAN granularity : 1;		
} PACKED gdt_flags_t;

typedef struct
{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t  base_middle;
	uint8_t  accessed        : 1;
	uint8_t  read_write      : 1;
	bool     dc_flag         : 1;
	uint8_t  executable      : 1;
	uint8_t  type            : 1;
	uint8_t  privilege_level : 2;
	uint8_t  present         : 1;
	uint8_t  limit_high      : 4;
	uint8_t  reserved        : 1;
	bool     long_mode       : 1;
	GDTSIZE  size            : 1;
	GDTGRAN  granularity     : 1;	
	uint8_t  base_high;
} PACKED gdt_entry_t;

typedef struct 
{
    uint16_t limit;
    uint32_t base;
} PACKED gdtr_t;

#define GDT_CONF    0
#define GDT_NONCONF 1
#define GDT_COUNT   3

#define KERNEL_CS 0x08
#define KERNEL_DS 0x10

extern uint32_t _gdtr;

void     gdt_init();
void     gdt_init_entries();
void     gdt_set_desc(uint8_t n, uint32_t base, uint32_t limit, gdt_access_t access, gdt_flags_t flags);