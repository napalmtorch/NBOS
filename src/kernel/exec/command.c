#include <kernel/exec/command.h>
#include <kernel/core/kernel.h>

#define CMD_COUNT 4096

command_t* _cmd_list;
uint32_t   _cmd_count;

void cmdhandler_execute_cmd(command_t* cmd, char** args, int args_count);
int cmdhandler_free_index();

int cmd_clear(int argc, char** argv)
{
    debug_info("CLEARED SCREEN");
    return 0;
}

void cmdhandler_init()
{
    _cmd_list = tmalloc(CMD_COUNT * sizeof(command_t), MEMTYPE_ARRAY);
    
    cmdhandler_register((command_t){ .name = "CLS", .usage = "cls [fg] [bg]", .help = "Clear the screen", .execute = cmd_clear });

    debug_ok("Initialized command handler");
}

void cmdhandler_register(command_t cmd)
{
    if (cmd.name == NULL || strlen(cmd.name) == 0) { return; }
    if (cmd.execute == NULL) { return; }

    int i = cmdhandler_free_index();
    if (i < 0 || i >= CMD_COUNT) { kpanic("Maximum amount of commands reached"); return; }

    _cmd_list[i] = cmd;
    _cmd_count++;
    debug_info("Registered command - HANDLER: 0x%8x, NAME: '%s'", cmd.execute, cmd.name);
}

int cmdhandler_execute(char* input)
{
    if (input == NULL) { return 0; }
    if (strlen(input) == 0) { return 0; }
    if (strwhite(input)) { return 0; }

    int args_count = 0;
    char** args = strsplit(input, ' ', &args_count);
    
    char* cmd = tmalloc(strlen(input), MEMTYPE_STRING);
    strcpy(cmd, args[0]);
    strupper(cmd);

    for (uint32_t i = 0; i < CMD_COUNT; i++)
    {
        if (_cmd_list[i].execute == NULL) { continue; }
        if (!strcmp(_cmd_list[i].name, cmd))
        {
            cmdhandler_execute_cmd(&_cmd_list[i], args, args_count);
            debug_ok("Executed command '%s'", cmd);
            return true;
        }
    }
    
    debug_error("Invalid command '%s'", cmd);
    return false;
}

void cmdhandler_execute_cmd(command_t* cmd, char** args, int args_count)
{
    lock();
    char* name = tmalloc(strlen(cmd->name) + 16, MEMTYPE_STRING);
    strcat(name, "cmd_");
    strcat(name, cmd->name);
    thread_t* t = thread_create(strlower(name), TSTACK_DEFAULT, cmd->execute, args_count, args);
    threadmgr_load(t);
    yieldf();
}

int cmdhandler_free_index()
{
    for (int i = 0; i < CMD_COUNT; i++)
    {
        if (_cmd_list[i].name == NULL && _cmd_list[i].execute == NULL) { return i; }
    }
    return NULL;
}