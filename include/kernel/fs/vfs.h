#pragma once
#include <lib/stdint.h>

typedef enum
{
    VFSTYPE_RAMFS,
    VFSTYPE_DISKFS,
} VFSTYPE;

typedef struct
{
    char     name[64];
    uint8_t* data;
    uint32_t size;
} file_t;

typedef struct
{
    char name[64];
} directory_t;

typedef struct
{
    VFSTYPE type;
    void*   fshost;
    char    letter;
    char    label[32];
} PACKED vfs_t;

void vfs_init();

vfs_t vfs_create(VFSTYPE type, void* host, char letter, char* label);
bool  vfs_register(vfs_t* vfs);
char  vfs_convert_path(char* path);
vfs_t* vfs_from_letter(char letter);

bool vfs_file_exists(char* fname);
bool vfs_dir_exists(char* path);

file_t vfs_file_open(char* fname);