#include <kernel/fs/ramfs.h>
#include <kernel/core/kernel.h>

ramfs_file_t NULL_RAMFS_FILE = { { 0 }, 0, 0 };

void ramfs_init(ramfs_t* ramfs, uint8_t* data, uint32_t size)
{
    if (ramfs == NULL) { return; }
    ramfs->data  = data;
    ramfs->size  = size;
    ramfs->info  = (ramfs_info_t*)ramfs->data;

    ramfs->info->files = (ramfs_file_t*)((uint32_t)ramfs->data + sizeof(ramfs_info_t));

    debug_ok("Initialized RAMFS - ADDR: 0x%8x, SIZE: %d MB", data, size / 1024 / 1024);
}

ramfs_file_t ramfs_read_file(ramfs_t* ramfs, char* filename)
{
    if (ramfs == NULL) { return NULL_RAMFS_FILE; }
    if (ramfs->data == NULL) { return NULL_RAMFS_FILE; }

    for (uint32_t i = 0; i < ramfs->info->count_max; i++)
    {
        if (ramfs->info->files[i].size == 0) { continue; }
        if (ramfs->info->files[i].data == NULL) { continue; }
        if (!strcmp(ramfs->info->files[i].name, filename)) 
        { 
            ramfs_file_t output;
            memset(&output, 0, sizeof(ramfs_file_t));
            strcpy(output.name, ramfs->info->files[i].name);
            output.data = (uint32_t)ramfs->data + ramfs->info->files[i].data;
            output.size = ramfs->info->files[i].size;
            debug_info("Successfully read RAMDISK file '%s'", filename);
            return output;
        }
    }

    debug_error("Unable to read RAMDISK file '%s'", filename);
    return NULL_RAMFS_FILE;
}

ramfs_file_t* ramfs_get_files(ramfs_t* ramfs)
{
    if (ramfs == NULL) { return NULL; }
    if (ramfs->data == NULL) { return NULL; }
    return ramfs->info->files;
}