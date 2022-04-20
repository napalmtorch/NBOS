#include <kernel/fs/vfs.h>
#include <kernel/core/kernel.h>

#define VFS_COUNT 16

const file_t NULL_FILE = (file_t){ .name = { 0 }, .data = NULL, .size = 0 };

vfs_t* _vfs_list[VFS_COUNT];
vfs_t  _ramfs_host;

int vfs_free_index();

void vfs_init()
{
    memset(&_vfs_list, 0, VFS_COUNT * sizeof(vfs_t*));

    uint32_t ramfs_size = ((multiboot_module_t*)MULTIBOOT->modules_addr)[0].address_end - ((multiboot_module_t*)MULTIBOOT->modules_addr)[0].address_start;
    ramfs_init(&RAMFS, (uint8_t*)((multiboot_module_t*)MULTIBOOT->modules_addr)[0].address_start, ramfs_size);
    _ramfs_host = vfs_create(VFSTYPE_RAMFS, &RAMFS, 'A', "RAMFS");
    if (!vfs_register(&_ramfs_host)) { kpanic("Unable to register RAMFS"); return; }

    debug_ok("Initialized VFS management system");
}

vfs_t vfs_create(VFSTYPE type, void* host, char letter, char* label)
{
    vfs_t vfs = (vfs_t){ .type = type, .fshost = host, .letter = letter }; 
    for (int i = 0; i < strlen(label); i++) { if (i < sizeof(vfs.label)) { vfs.label[i] = label[i]; } }
    debug_ok("Created virtual FS - TYPE: 0x%2x, HOST: 0x%8x, LETTER: %c, LABEL: '%s'", type, host, letter, label);
    return vfs;
}

bool vfs_register(vfs_t* vfs)
{   
    if (vfs == NULL) { return false; }
    int i = vfs_free_index();
    if (i < 0 || i >= VFS_COUNT) { return false; }
    _vfs_list[i] = vfs;
    debug_info("Registered VFS - TYPE: 0x%2x, HOST: 0x%8x, LETTER: %c, LABEL: '%s'", vfs->type, vfs->fshost, vfs->letter, vfs->label);
    return true;
}

char vfs_convert_path(char* path)
{
    int len = strlen(path);
    if (len < 3) { return 0; }
    if (!isalpha(path[0])) { return 0; }
    if (path[1] != ':' || path[2] != '/') { return 0; }
    char* temp = tmalloc(len + 1, MEMTYPE_STRING);
    char letter = path[0];
    strcpy(temp, path + 3);
    memset(path, 0, len);
    strcpy(path, temp);
    free(temp);
    return letter;
}

vfs_t* vfs_from_letter(char letter)
{
    for (int i = 0; i < VFS_COUNT; i++)
    {
        if (_vfs_list[i] == NULL) { continue; }
        if (_vfs_list[i]->letter == letter) { return _vfs_list[i]; }
    }
    return NULL;
}

bool vfs_file_exists(char* fname)
{
    if (fname == NULL) { return false; }

    char letter = vfs_convert_path(fname);
    if (letter == 0) { return false; }

    vfs_t* vfs = vfs_from_letter(letter);
    if (vfs == NULL) { return false; }

    if (vfs->type == VFSTYPE_RAMFS)
    {
        ramfs_t* ramfs = vfs->fshost;
        for (int i = 0; i < RAMFS.info->count_max; i++)
        {
            if (!strcmp(ramfs->info->files[i].name, fname)) { return true; }
        }
        return false;
    }

    if (vfs->type == VFSTYPE_DISKFS)
    {
        return false;
    }
    return false;
}

bool vfs_dir_exists(char* path)
{
    if (path == NULL) { return false; }

    char letter = vfs_convert_path(path);
    if (letter == 0) { return false; }

    vfs_t* vfs = vfs_from_letter(letter);
    if (vfs == NULL) { return false; }

    if (vfs->type == VFSTYPE_RAMFS) { return false; }

    if (vfs->type == VFSTYPE_DISKFS) 
    {
        return false;
    }
    return false;
}

file_t vfs_file_open(char* fname)
{
    if (fname == NULL) { return NULL_FILE; }

    char letter = vfs_convert_path(fname);
    if (letter == 0) { return NULL_FILE; }

    vfs_t* vfs = vfs_from_letter(letter);
    if (vfs == NULL) { return NULL_FILE; }

    if (vfs->type == VFSTYPE_RAMFS)
    {
        ramfs_t* ramfs = vfs->fshost;
        ramfs_file_t ramfile = ramfs_read_file(ramfs, fname);
        if (ramfile.data == NULL) { return NULL_FILE; }

        file_t file;
        memset(&file, 0, sizeof(file_t));
        strcpy(file.name, ramfile.name);
        file.size = ramfile.size;
        file.data = tmalloc(file.size, MEMTYPE_ARRAY);
        memcpy(file.data, ramfile.data, file.size);
        return file;
    }

    if (vfs->type == VFSTYPE_DISKFS)
    {
        return NULL_FILE;
    }

    return NULL_FILE;
}

int vfs_free_index()
{
    for (int i = 0; i < VFS_COUNT; i++) { if (_vfs_list[i] == NULL) { return i; } }
    return -1;
}