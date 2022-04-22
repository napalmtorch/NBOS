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
    cmdhandler_register((command_t){ .name = "CD", .usage = "cd [path]", .help = "Set the current directory", .execute = cmd_cd });
    cmdhandler_register((command_t){ .name = "DIR", .usage = "dir [path]", .help = "List contents of directory", .execute = cmd_dir });

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
    
    if (argc == 3 || strlen(argv[3]) == 0) { term_printcaret(t); return 0; }

    char* p = tmalloc(strlen(argv[3]) + 1, MEMTYPE_STRING);
    strcpy(p, argv[3]);

    char* fname = term_getfullpath(t, p);

    DEBUG old = debug_getmode();
    debug_setmode(DEBUG_TERMINAL);
    debug_setterm(t);
    gs_tokenizer_t tokenizer = gs_tokenize_file(fname);
    if (tokenizer.toks == NULL) { term_println_fg(t, "Tokenizer failed", COL32_TOMATO); }
    else { term_println_fg(t, "Program finished", COL32_LIME); }
    debug_setmode(old);

    free(p);
    free(fname);
    term_printcaret(t);
    return 0;
}

int cmd_dir(int argc, char** argv)
{
    terminal_t* t = argv[1];
    char* path = argv[3];

    // no path specified
    if (argc == 3 || strlen(path) == 0) 
    { 
        char* p = tmalloc(strlen(t->path) + 1, MEMTYPE_STRING);
        strcpy(p, t->path);

        int file_count = 0;
        char** files   = vfs_get_files(p, &file_count);

        for (int i = 0; i < file_count; i++) { term_println(t, files[i]); free(files[i]); }

        free(p);
        term_printcaret(t); 
        return 0; 
    }

    char* p = term_getfullpath(t, path);
    if (!vfs_dir_exists(p)) { term_printf(t, "Invalid directory '%s'\n", p); free(p); term_printcaret(t); return NULL; }

    int file_count = 0;
    char** files   = vfs_get_files(p, &file_count);

    for (int i = 0; i < file_count; i++) { term_println(t, files[i]); free(files[i]); }

    term_printcaret(t); 
    return 0; 
}

int cmd_cd(int argc, char** argv)
{
    terminal_t* t = argv[1];
    char* path = argv[3];

    if (argc == 3 || strlen(path) == 0) { term_printcaret(t); return 0; }

    char* p = term_getfullpath(t, path);
    if (!vfs_dir_exists(p)) { term_printf(t, "Invalid directory '%s'\n", p); free(p); term_printcaret(t); return NULL; }

    free(t->path);
    t->path = p;
    term_printcaret(t);
    return 0;
}

int cmd_view(int argc, char** argv)
{
    
}