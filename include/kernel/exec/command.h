#pragma once
#include <lib/stdint.h>
#include <lib/stdarg.h>
#include <kernel/exec/thread.h>
#include <kernel/services/terminal.h>

typedef struct
{
    char*          name;
    char*          help;
    char*          usage;
    thread_entry_t execute;
} PACKED command_t;

void cmdhandler_init();
void cmdhandler_register(command_t cmd);
int  cmdhandler_execute(char* input, terminal_t* term);

int cmd_clear(int argc, char** argv);
int cmd_help(int argc, char** argv);
int cmd_exec(int argc, char** argv);
