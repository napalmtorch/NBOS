#pragma once
#include <lib/stdint.h>
#include <lib/stream.h>

typedef enum
{
    TOKEN_INVALID,
    TOKEN_ID,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_SEMICOL,
    TOKEN_COMMA,
    TOKEN_PERIOD,
    TOKEN_PAREN_L,
    TOKEN_PAREN_R,
    TOKEN_SCOPE_L,
    TOKEN_SCOPE_R,
    TOKEN_INDEX_L,
    TOKEN_INDEX_R,
    TOKEN_EQL,
    TOKEN_GREATER,
    TOKEN_LESS,

    TOKEN_NULL,
    TOKEN_LITERAL_DEC,
    TOKEN_LITERAL_HEX,
    TOKEN_LITERAL_BOOL,
    TOKEN_LITERAL_CHAR,
    TOKEN_LITERAL_STRING,
} TOKEN;

typedef struct
{
    int   line;
    char* value;
    TOKEN type;
} PACKED gs_token_t;

typedef struct
{
    gs_token_t* toks;
    stream_t    input;
    int         tok_count;
    int         line;
    char*       word;

} PACKED gs_tokenizer_t;

static const char* TOK_KEYWORDS[]  = { "using", "class", "func", "break", "continue", "return", "if", "elseif", "else", "for", "while" };
static const char  TOK_OPERATORS[] = { '+', '-', '*', '/', '&', '|', '^' };
static const char  TOK_SYMBOLS[]   = { ';', ',', '.', '(', ')', '{', '}', '[', ']', '=', '>', '<' };

gs_tokenizer_t gs_tokenize_file(char* fname);
gs_tokenizer_t gs_tokenize(char* input, int size);

bool gs_is_keyword(char* value);
bool gs_is_op(char c);
bool gs_is_opstr(char* str);
bool gs_is_sym(char c);
bool gs_is_symstr(char* str);

TOKEN gs_symtype(char c);