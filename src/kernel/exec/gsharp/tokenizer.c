#include <kernel/exec/gsharp/tokenizer.h>
#include <kernel/core/kernel.h>

#define TOK_WORDSZ 32768

bool tokenizer_run(gs_tokenizer_t* tokenizer);
bool tokenizer_handle(gs_tokenizer_t* tokenizer, char c);
bool tokenizer_handle_word(gs_tokenizer_t* tokenizer);
bool tokenizer_handle_keyword(gs_tokenizer_t* tokenizer);
bool tokenizer_handle_operator(gs_tokenizer_t* tokenizer, char c);
bool tokenizer_handle_symbol(gs_tokenizer_t* tokenizer, char c);
bool tokenizer_handle_string(gs_tokenizer_t* tokenizer);
bool tokenizer_handle_char(gs_tokenizer_t* tokenizer);
bool tokenizer_handle_dec(gs_tokenizer_t* tokenizer);
bool tokenizer_handle_hex(gs_tokenizer_t* tokenizer);
bool tokenizer_handle_bool(gs_tokenizer_t* tokenizer);
void tokenizer_addtok(gs_tokenizer_t* tokenizer, gs_token_t tok);

gs_tokenizer_t gs_tokenize_file(char* fname)
{
    file_t file = vfs_file_open(fname);
    if (file.data == NULL) { debug_error("Unable to load file '%s' into tokenizer", fname); return (gs_tokenizer_t){ 0 }; }
    debug_info("Tokenizer loaded file '%s'", fname);
    return gs_tokenize(file.data, file.size);
}

gs_tokenizer_t gs_tokenize(char* input, int size)
{
    gs_tokenizer_t tokenizer;
    tokenizer.input = stream_create(size + 1);
    strcpy(tokenizer.input.data, input);
    if (!tokenizer_run(&tokenizer)) { return (gs_tokenizer_t){ 0 }; }
    
    return tokenizer;
}

bool tokenizer_run(gs_tokenizer_t* tokenizer)
{
    tokenizer->tok_count = 0;
    tokenizer->line      = 1;
    tokenizer->word      = tmalloc(TOK_WORDSZ, MEMTYPE_STRING);
    tokenizer->toks      = NULL;

    bool error = false;
    while (true)
    {
        char c = stream_next(&tokenizer->input);
        if (!tokenizer_handle(tokenizer, c)) { error = true; break; }
        if (stream_isdone(&tokenizer->input)) { break; }
        yield();
    }

    if (error) 
    { 
        free(tokenizer->toks);
        free(tokenizer->word);
        stream_dispose(&tokenizer->input);
        tokenizer->toks = NULL;
        tokenizer->word = NULL;
        debug_error("Tokenizer failed"); 
    }
    else { debug_ok("Tokenizer finished"); }

    yield();
    return !error;
}

bool tokenizer_handle(gs_tokenizer_t* tokenizer, char c)
{
    if (gs_is_op(c)) { tokenizer_handle_operator(tokenizer, c); return true; }
    if (gs_is_sym(c)) { tokenizer_handle_symbol(tokenizer, c); return true; }

    switch (c)
    {
        case '\t': { memset(tokenizer->word, 0, TOK_WORDSZ); return true; }
        case '\'': { if (!tokenizer_handle_char(tokenizer)) { return false; } return true; }
        case '\"': { if (!tokenizer_handle_string(tokenizer)) { return false; } return true; }
        case ' ':  { if (!tokenizer_handle_word(tokenizer)) { return false; } return true; }
        case '\n': { if (!tokenizer_handle_word(tokenizer)) { return false; } tokenizer->line++; return true; }
        default: { stradd(tokenizer->word, c); return true; }
    }
    return false;
}

bool tokenizer_handle_word(gs_tokenizer_t* tokenizer)
{
    if (strlen(tokenizer->word) == 0) { return true; }
    if (!strcmp(tokenizer->word, " ")) { memset(tokenizer->word, 0, TOK_WORDSZ); return true; }

    if (gs_is_keyword(tokenizer->word)) { return tokenizer_handle_keyword(tokenizer); }

    if (str_isdec(tokenizer->word)) { return tokenizer_handle_dec(tokenizer); }
    if (str_ishex(tokenizer->word, true)) { return tokenizer_handle_hex(tokenizer); }
    if (!strcmp(tokenizer->word, "true") || !strcmp(tokenizer->word, "false")) { return tokenizer_handle_bool(tokenizer); }

    char* word = tmalloc(strlen(tokenizer->word) + 1, MEMTYPE_STRING);
    strcpy(word, tokenizer->word);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_ID, .value = word });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

bool tokenizer_handle_keyword(gs_tokenizer_t* tokenizer)
{
    char* word = tmalloc(strlen(tokenizer->word) + 1, MEMTYPE_STRING);
    strcpy(word, tokenizer->word);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_KEYWORD, .value = word });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

bool tokenizer_handle_operator(gs_tokenizer_t* tokenizer, char c)
{
    tokenizer_handle_word(tokenizer);
    char* val = tmalloc(2, MEMTYPE_STRING);
    val[0] = c;
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_OPERATOR, .value = val });
    return true;
}

bool tokenizer_handle_symbol(gs_tokenizer_t* tokenizer, char c)
{
    tokenizer_handle_word(tokenizer);
    char* val = tmalloc(2, MEMTYPE_STRING);
    val[0] = c;
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = gs_symtype(c), .value = val });
    return true;
}

bool tokenizer_handle_string(gs_tokenizer_t* tokenizer)
{
    char* str = tmalloc(tokenizer->input.size, MEMTYPE_STRING);
    while (true)
    {
        char c = stream_next(&tokenizer->input);
        if (c == '\"' || c == 0) { break; }
        stradd(str, c);
        yield();
    }

    char* val = tmalloc(strlen(str) + 1, MEMTYPE_STRING);
    strcpy(val, str);
    free(str);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_LITERAL_STRING, .value = val });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

bool tokenizer_handle_char(gs_tokenizer_t* tokenizer)
{
    char* str = tmalloc(tokenizer->input.size, MEMTYPE_STRING);
    while (true)
    {
        char c = stream_next(&tokenizer->input);
        if (c == '\'' || c == 0) { break; }
        stradd(str, c);
        yield();
    }

    char* val = tmalloc(strlen(str) + 1, MEMTYPE_STRING);
    strcpy(val, str);
    free(str);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_LITERAL_CHAR, .value = val });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

bool tokenizer_handle_dec(gs_tokenizer_t* tokenizer)
{
    char* val = tmalloc(strlen(tokenizer->word) + 1, MEMTYPE_STRING);
    strcpy(val, tokenizer->word);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_LITERAL_DEC, .value = val });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

bool tokenizer_handle_hex(gs_tokenizer_t* tokenizer)
{
    char* val = tmalloc(strlen(tokenizer->word) + 1, MEMTYPE_STRING);
    strcpy(val, tokenizer->word + 2);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_LITERAL_HEX, .value = val });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

bool tokenizer_handle_bool(gs_tokenizer_t* tokenizer)
{
    char* val = tmalloc(strlen(tokenizer->word) + 1, MEMTYPE_STRING);
    strcpy(val, tokenizer->word);
    tokenizer_addtok(tokenizer, (gs_token_t){ .line = tokenizer->line, .type = TOKEN_LITERAL_BOOL, .value = val });
    memset(tokenizer->word, 0, TOK_WORDSZ);
    return true;
}

void tokenizer_addtok(gs_tokenizer_t* tokenizer, gs_token_t tok)
{
    gs_token_t* toks = tmalloc((tokenizer->tok_count + 1) * sizeof(gs_token_t), MEMTYPE_ARRAY);
    
    if (tokenizer->toks != NULL)
    {
        memcpy(toks, tokenizer->toks, tokenizer->tok_count * sizeof(gs_token_t));
        free(tokenizer->toks);
    }
    tokenizer->toks = toks;
    tokenizer->toks[tokenizer->tok_count] = tok;
    tokenizer->tok_count++;
    debug_info("Added token - LINE: %d TYPE: 0x%2x VAL: '%s'", tok.line, tok.type, tok.value);
}

bool gs_is_keyword(char* value)
{
    const int count = sizeof(TOK_KEYWORDS) / sizeof(char*);
    for (int i = 0; i < count; i++) { if (!strcmp(TOK_KEYWORDS[i], value)) { return true; } }
    return false;
}

bool gs_is_op(char c)
{
    const int count = sizeof(TOK_OPERATORS) / sizeof(char);
    for (int i = 0; i < count; i++) { if (TOK_OPERATORS[i] == c) { return true; } }
    return false;
}

bool gs_is_opstr(char* str)
{
    if (strlen(str) > 1) { return false; }
    return gs_is_op(str[0]);
}

bool gs_is_sym(char c)
{
    const int count = sizeof(TOK_SYMBOLS) / sizeof(char);
    for (int i = 0; i < count; i++) { if (TOK_SYMBOLS[i] == c) { return true; } }
    return false;
}

bool gs_is_symstr(char* str)
{
    if (strlen(str) > 1) { return false; }
    return gs_is_sym(str[0]);
}

TOKEN gs_symtype(char c)
{
    const int count = sizeof(TOK_SYMBOLS) / sizeof(char);
    for (int i = 0; i < count; i++) { if (TOK_SYMBOLS[i] == c) { return TOKEN_SEMICOL + i; } }
    return TOKEN_INVALID;
}