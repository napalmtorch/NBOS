#include <kernel/exec/command.h>
#include <kernel/core/kernel.h>

#define CMD_COUNT 4096

command_t* _cmd_list;
uint32_t   _cmd_count;

void cmdhandler_execute_cmd(command_t* cmd, terminal_t* term, char** args, int args_count);
int cmdhandler_free_index();

void cmdhandler_init()
{
    _cmd_list = tmalloc(CMD_COUNT * sizeof(command_t), MEMTYPE_ARRAY);
    
    cmdhandler_register((command_t){ .name = "CLS",  .usage = "cls [fg] [bg]", .help = "Clear the screen", .execute = cmd_clear });
    cmdhandler_register((command_t){ .name = "HELP", .usage = "help [-u : usage]", .help = "Show list of commands", .execute = cmd_help });
    cmdhandler_register((command_t){ .name = "EXEC", .usage = "exec [fname] [args]", .help = "Execute a gsharp file", .execute = cmd_exec });

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

int cmdhandler_execute(char* input, terminal_t* term)
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
            debug_ok("Executing command '%s'", cmd);
            cmdhandler_execute_cmd(&_cmd_list[i], term, args, args_count);
            for (int i = 0; i < args_count; i++) { free(args[i]); }
            free(args);
            free(cmd);
            return true;
        }
    }
    
    debug_error("Invalid command '%s'", cmd);
    for (int i = 0; i < args_count; i++) { free(args[i]); }
    free(args);
    free(cmd);
    return false;
}

void cmdhandler_execute_cmd(command_t* cmd, terminal_t* term, char** args, int args_count)
{
    char* name = tmalloc(strlen(cmd->name) + 16, MEMTYPE_STRING);
    strcat(name, "cmd_");
    strcat(name, cmd->name);
        
    char** newargs = tmalloc((args_count + 1) * sizeof(char*), MEMTYPE_PTRARRAY);
    newargs[0] = term;
    for (int i = 0; i < args_count; i++) { newargs[i + 1] = args[i]; }

    thread_t* t = thread_create(strlower(name), TSTACK_DEFAULT, cmd->execute, args_count + 1, newargs);
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

int cmd_clear(int argc, char** argv)
{
    terminal_t* t = argv[1];
    if (t == NULL) { return 1; }

    term_clear(t);
    term_printcaret(t);
    return 0;
}

int cmd_help(int argc, char** argv)
{
    terminal_t* t = argv[1];
    if (t == NULL) { return 1; }

    for (int i = 0; i < CMD_COUNT; i++)
    {
        if (_cmd_list[i].execute == NULL) { continue; }
        term_print(t, _cmd_list[i].name);
        t->cx = 24;
        term_println_fg(t, _cmd_list[i].help, COL32_GRAY);
    }

    term_printcaret(t);
    return 0;
}

int cmd_exec(int argc, char** argv)
{
    terminal_t* t = argv[1];
    char* fname = tmalloc(32768, MEMTYPE_STRING);
    for (int i = 3; i < argc; i++) { strcat(fname, argv[i]); }
    debug_info("FNAME: '%s'", fname);
    
    DEBUG old = debug_getmode();
    debug_setmode(DEBUG_TERMINAL);
    debug_setterm(t);
    gs_tokenizer_t tokenizer = gs_tokenize_file(fname);
    if (tokenizer.toks == NULL) { term_println_fg(t, "Tokenizer failed", COL32_TOMATO); }
    else { term_println_fg(t, "Program finished", COL32_LIME); }
    debug_setmode(old);
    term_printcaret(t);
    return 0;
}