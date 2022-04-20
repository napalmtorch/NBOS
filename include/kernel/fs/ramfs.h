#pragma once
#include <lib/stdint.h>
#include <lib/stdlib.h>
#include <lib/stdio.h>

typedef struct
{
    char     name[32];
    uint8_t* data;
    uint32_t size;
} PACKED ramfs_file_t;

typedef struct
{
    uint32_t      count;
    uint32_t      count_max;
    uint32_t      cur_address;
    ramfs_file_t* files;
} PACKED ramfs_info_t;

typedef struct
{
    uint8_t*      data;
    uint32_t      size;
    ramfs_info_t* info;
} PACKED ramfs_t;

void ramfs_init(ramfs_t* ramfs, uint8_t* data, uint32_t size);
ramfs_file_t ramfs_read_file(ramfs_t* ramfs, char* filename);
ramfs_file_t* ramfs_get_files(ramfs_t* ramfs);