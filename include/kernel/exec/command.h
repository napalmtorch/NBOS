#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <kernel/exec/thread.h>

typedef struct
{
    char*          name;
    char*          help;
    char*          usage;
    thread_entry_t execute;
} PACKED command_t;

void cmdhandler_init();
void cmdhandler_register(command_t cmd);
int  cmdhandler_execute(char* input);